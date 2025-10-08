# Transposing Flame/Flare Pybox ComfyUI Nodes to OpenFX Plugins

## Executive Summary

This document provides a comprehensive guide for converting the Python-based Flame/Flare Pybox ComfyUI node system into native OpenFX plugins. The original system consists of three Python repositories that enable Flame/Flare to connect to a ComfyUI REST server for AI-powered image processing. This transposition will create equivalent OFX plugins that can be used in any OpenFX-compatible host application (Flame, Nuke, Resolve, etc.).

---

## Original System Architecture

### Component Overview

The original Python system has three layers:

1. **ComfyUI Client** (`flame_comfyui_client`)
   - Low-level REST API communication with ComfyUI server
   - WebSocket connection for real-time status monitoring
   - Image file I/O management
   - Model discovery and management

2. **Base Pybox ComfyUI Class** (`flame_comfyui_pybox`)
   - Abstract base class inheriting from `pybox.BaseClass`
   - Orchestrates workflow execution and state management
   - Handles file I/O to/from shared network directories
   - Provides extensibility framework for specific AI operations

3. **Concrete Implementation** (`flame_comfyui_segmentation`)
   - SAM2 (Segment Anything Model 2) segmentation node
   - Extends base class with specific workflow and parameters
   - Demonstrates pattern for creating derived nodes

---

## Detailed Analysis of Original System

### 1. ComfyUI Client (`comfyui_client.py`)

#### Core Functionality

**REST API Communication:**

```python
# Key endpoints
POST /prompt          # Submit workflow for execution
GET  /history/{id}    # Retrieve execution results
POST /interrupt       # Cancel running workflow
GET  /view            # Fetch generated images
WS   ws://{server}/ws # Real-time status updates
```

**Key Functions:**

- `queue_prompt(prompt, client_id)` - Submit workflow to server
- `get_history(prompt_id)` - Retrieve execution results
- `get_images(ws, prompt)` - Download output images via WebSocket
- `interrupt_execution(prompt, client_id)` - Cancel workflow
- `find_models(root_dirs)` - Discover available AI models
- `list_files(dir, basename, ...)` - File system operations

**Data Flow:**

1. Client generates unique ID
2. Workflow JSON submitted via HTTP POST
3. WebSocket monitors execution status
4. Images written to shared filesystem
5. Client reads results from output directory

**Error Handling:**

- `ConnectionRefusedError` - Server unavailable
- `URLError` - Network issues
- Returns empty dictionaries on failure

#### Configuration

```python
server_address = "hostname:port"
input_dir = "<COMFYUI_SERVER_MOUNTING>/in/<PROJECT>/"
output_dir = "<COMFYUI_SERVER_MOUNTING>/out/<PROJECT>/<VERSION>/"
```

---

### 2. Base Pybox ComfyUI Class (`pybox_comfyui.py`)

#### Class Architecture

**Inheritance Hierarchy:**

```
pybox.BaseClass (Flame API)
    └── ComfyUIBaseClass (abstract)
        └── Concrete nodes (e.g., SegmentationNode)
```

**Core Components:**

1. **Initialization & Setup:**

```python
def initialize():
    # Set up server connection
    # Initialize ComfyUI client
    # Load workflow JSON template
    # Configure file I/O paths
    # Set up layer management
```

2. **UI Management:**

```python
def setup_ui():
    # Create server configuration widgets
    # Add model selection dropdowns
    # Define processing parameters
    # Set up version management controls
```

3. **Workflow Execution:**

```python
def execute():
    # Handle version management
    # Process UI interactions
    # Trigger workflow execution
    # Monitor processing state
```

4. **File I/O Handling:**

```python
# Input: Write frames to shared network directory
input_path = f"{server_mount}/in/{project}/{node_type}/"
# Output: Read processed results
output_path = f"{server_mount}/out/{project}/{node_type}/{version}/"
```

**Processing States:**

```python
class ProcessingState(Enum):
    Idle = "Idle"
    Executing = "Executing"
    Processed = "Processed"
```

**Layer Management:**

```python
class LayerType(Enum):
    Front = "Front"
    Back = "Back"
    Matte = "Matte"
    Result = "Result"
```

#### Abstract Methods (Must be implemented by derived classes)

- `set_models()` - Configure available AI models
- `workflow_setup()` - Prepare workflow for execution
- `load_workflow()` - Load and parse workflow JSON
- `init_ui()` - Create node-specific UI elements

---

### 3. Segmentation Implementation (`comfyui_segmentation_segment_anything.py`)

#### Implementation Details

**Class Definition:**

```python
class SegmentationNode(ComfyUIBaseClass):
    def __init__(self):
        super().__init__()
        self.workflow_path = "path/to/workflow.json"
        self.sam_model = None
        self.dino_model = None
        self.threshold = 0.3
        self.resolution = 1080
        self.prompt = "foreground"
```

**Parameter Configuration:**

- **SAM Model** - Dropdown of available Segment Anything models
- **DINO Model** - Dropdown of GroundingDINO models for detection
- **Threshold** - Segmentation confidence threshold (0.0-1.0)
- **Resolution** - Processing resolution
- **Text Prompt** - Natural language description of target object

**Processing Workflow:**

1. Load input frame from Flame batch
2. Write as EXR to shared input directory
3. Update workflow JSON with current parameters
4. Submit workflow to ComfyUI server
5. Monitor execution via WebSocket
6. Read output EXR files (result + matte)
7. Return layers to Flame batch

#### ComfyUI Workflow Structure (`workflow_api.json`)

**Node Graph:**

```json
{
  "1": {  // LoadEXR - Input loader
    "class_type": "LoadEXR",
    "inputs": {
      "image": "input_path.exr",
      "color_space": "linear_to_srgb"
    }
  },
  "17": {  // GroundingDinoModelLoader
    "class_type": "GroundingDinoModelLoader (segment anything)",
    "inputs": {
      "model_name": "GroundingDINO_SwinT_OGC (694MB)"
    }
  },
  "18": {  // SAMModelLoader
    "class_type": "SAMModelLoader (segment anything)",
    "inputs": {
      "model_name": "sam_vit_h (2.56GB)"
    }
  },
  "16": {  // GroundingDinoSAMSegment - Core processing
    "class_type": "GroundingDinoSAMSegment (segment anything)",
    "inputs": {
      "prompt": "foreground",
      "threshold": 0.3,
      "sam_model": ["18", 0],
      "grounding_dino_model": ["17", 0],
      "image": ["1", 0]
    }
  },
  "20": {  // MaskToImage
    "class_type": "MaskToImage",
    "inputs": {
      "mask": ["16", 1]
    }
  },
  "24": {  // SAMPreprocessor
    "class_type": "SAMPreprocessor",
    "inputs": {
      "resolution": 1080,
      "image": ["20", 0]
    }
  },
  "27": {  // SaveEXR - Result output
    "class_type": "SaveEXR",
    "inputs": {
      "filename_prefix": "Result",
      "images": ["16", 0],
      "color_space": "srgb_to_linear"
    }
  },
  "23": {  // SaveEXR - Matte output
    "class_type": "SaveEXR",
    "inputs": {
      "filename_prefix": "OutMatte",
      "images": ["24", 0],
      "color_space": "srgb_to_linear"
    }
  }
}
```

**Data Flow:**

```
Input EXR → DINO Detection → SAM Segmentation → Result Image
                                            ↓
                                        Mask → Preprocessor → Matte Image
```

---

## OpenFX Architecture Overview

### Core Concepts

**Plugin Structure:**

- Written in C/C++ for performance and compatibility
- Uses property-based API for host communication
- Supports multiple bit depths (8-bit, 16-bit, float)
- Clip-based input/output system
- Parameter system for UI controls

**Processing Model:**

```cpp
class OFXPlugin : public OFX::ImageEffect {
    // Lifecycle
    describe()              // Define plugin capabilities
    describeInContext()     // Define clips and parameters
    createInstance()        // Instantiate plugin

    // Processing
    render()                // Main image processing
    getRegionOfDefinition() // Define output bounds
    getRegionsOfInterest()  // Define input requirements

    // Parameters
    paramChanged()          // Handle parameter updates
    getClipPreferences()    // Define pixel format preferences
};
```

**Support Library Benefits:**

- C++ wrapper classes around C API
- Object-oriented design
- Template-based multi-bit-depth processing
- Built-in multithread processing support
- Exception handling and error management

---

## Transposition Strategy

### Design Goals

1. **Maintain Functionality** - Preserve all features from Python version
2. **Improve Performance** - Native C++ execution
3. **Broaden Compatibility** - Support all OFX hosts, not just Flame
4. **Simplify Deployment** - Single binary bundle vs Python dependencies
5. **Enhance Reliability** - Compiled code, static typing, error handling

### Architecture Mapping

#### Python → C++ Component Mapping

| Python Component | OFX Equivalent | Implementation |
|-----------------|----------------|----------------|
| `ComfyUIClient` | C++ REST Client | libcurl/cpp-httplib |
| `pybox.BaseClass` | `OFX::ImageEffect` | Support Library |
| `setup_ui()` | `describeInContext()` | Parameter descriptors |
| `process_frame()` | `render()` | Image processing loop |
| Workflow JSON | Embedded resource | JSON parsing library |
| File I/O | OpenImageIO/EXR | Image file handling |
| WebSocket | WebSocket++ | Status monitoring |
| Model discovery | Filesystem scan | Platform-agnostic paths |

---

## Detailed Implementation Plan

### Phase 1: Foundation Components

#### 1.1 ComfyUI REST Client (C++)

**Dependencies:**

- **cpp-httplib** - Header-only HTTP/HTTPS library
- **WebSocket++** - WebSocket client implementation
- **nlohmann/json** - JSON parsing and generation

**Class Structure:**

```cpp
class ComfyUIClient {
public:
    ComfyUIClient(const std::string& serverAddress);
    ~ComfyUIClient();

    // Core API
    std::string queuePrompt(const json& workflow, const std::string& clientId);
    json getHistory(const std::string& promptId);
    bool interruptExecution(const std::string& clientId);
    std::vector<ImageData> getImages(const std::string& promptId);

    // Configuration
    void setServerAddress(const std::string& address);
    void setInputDirectory(const std::string& path);
    void setOutputDirectory(const std::string& path);

    // Model management
    std::vector<std::string> findModels(const std::string& modelType);

    // Connection management
    bool testConnection();
    void connect();
    void disconnect();

private:
    std::string m_serverAddress;
    std::string m_inputDir;
    std::string m_outputDir;
    std::unique_ptr<WebSocketClient> m_wsClient;
    std::string m_clientId;

    // Helper methods
    json makeRequest(const std::string& endpoint, const json& data);
    std::string generateClientId();
};
```

**Key Implementation Details:**

**HTTP Requests:**

```cpp
json ComfyUIClient::queuePrompt(const json& workflow, const std::string& clientId) {
    httplib::Client client(m_serverAddress);

    json payload = {
        {"prompt", workflow},
        {"client_id", clientId}
    };

    auto res = client.Post("/prompt", payload.dump(), "application/json");

    if (res && res->status == 200) {
        return json::parse(res->body);
    }

    throw std::runtime_error("Failed to queue prompt");
}
```

**WebSocket Status Monitoring:**

```cpp
void ComfyUIClient::monitorExecution(const std::string& promptId,
                                     ProgressCallback callback) {
    m_wsClient->connect(m_serverAddress + "/ws?clientId=" + m_clientId);

    m_wsClient->setMessageHandler([&](const std::string& message) {
        json msg = json::parse(message);

        if (msg["type"] == "executing") {
            // Update progress
            callback(msg["data"]["node"], msg["data"]["progress"]);
        }
        else if (msg["type"] == "execution_complete") {
            // Execution finished
            return true;
        }
    });

    m_wsClient->run();
}
```

#### 1.2 Base ComfyUI OFX Plugin Class

**Class Hierarchy:**

```cpp
OFX::ImageEffect
    └── ComfyUIBasePlugin (abstract)
        └── ComfyUISegmentationPlugin
        └── ComfyUIUpscalePlugin
        └── [Other AI nodes...]
```

**Base Class Definition:**

```cpp
class ComfyUIBasePlugin : public OFX::ImageEffect {
protected:
    // Clips
    OFX::Clip *_srcClip;
    OFX::Clip *_dstClip;
    OFX::Clip *_maskClip;  // Optional mask output

    // Common parameters
    OFX::StringParam *_serverAddress;
    OFX::IntParam *_serverPort;
    OFX::StringParam *_sharedMountPath;
    OFX::StringParam *_projectName;
    OFX::ChoiceParam *_processingMode;
    OFX::BooleanParam *_enableCache;
    OFX::StringParam *_cacheDirectory;

    // ComfyUI client
    std::unique_ptr<ComfyUIClient> _comfyClient;

    // State management
    enum ProcessingState {
        Idle,
        Queuing,
        Processing,
        Completed,
        Error
    };
    ProcessingState _state;
    std::string _lastPromptId;

public:
    ComfyUIBasePlugin(OfxImageEffectHandle handle);
    virtual ~ComfyUIBasePlugin();

    // OFX lifecycle
    virtual void changedParam(const OFX::InstanceChangedArgs &args,
                             const std::string &paramName) override;
    virtual void render(const OFX::RenderArguments &args) override;
    virtual bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments &args,
                                      OfxRectD &rod) override;

    // Template method pattern - derived classes implement these
    virtual json buildWorkflow() = 0;
    virtual void setupParameters(OFX::ImageEffectDescriptor &desc,
                                OFX::ContextEnum context) = 0;
    virtual std::vector<std::string> getRequiredModels() = 0;
    virtual void processOutput(const std::vector<ImageData>& results) = 0;

protected:
    // Workflow execution
    void executeWorkflow(const OFX::RenderArguments &args);
    void waitForCompletion(const std::string& promptId);

    // File I/O
    std::string writeInputImage(OFX::Image* img, int frame);
    void readOutputImages(const std::string& outputDir,
                         std::vector<ImageData>& results);

    // Model management
    void discoverModels();
    bool validateModelAvailability();

    // Error handling
    void handleComfyError(const std::string& error);
    void reportProgress(float progress, const std::string& status);
};
```

**Core Implementation Methods:**

**render() Implementation:**

```cpp
void ComfyUIBasePlugin::render(const OFX::RenderArguments &args) {
    // 1. Fetch input image
    std::unique_ptr<OFX::Image> src(_srcClip->fetchImage(args.time));
    if (!src.get()) {
        OFX::throwSuiteStatusException(kOfxStatFailed);
    }

    // 2. Write input to shared directory
    std::string inputPath = writeInputImage(src.get(), args.time);

    // 3. Build workflow JSON (implemented by derived class)
    json workflow = buildWorkflow();

    // Update workflow with input path
    workflow["1"]["inputs"]["image"] = inputPath;

    // 4. Submit to ComfyUI server
    std::string promptId = _comfyClient->queuePrompt(workflow,
                                                     _comfyClient->getClientId());

    // 5. Wait for completion (with progress updates)
    waitForCompletion(promptId);

    // 6. Read results from output directory
    std::vector<ImageData> results;
    readOutputImages(_comfyClient->getOutputDirectory(), results);

    // 7. Process results (derived class handles specifics)
    processOutput(results);

    // 8. Write to OFX output clip
    std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));
    if (dst.get()) {
        copyImageData(results[0], dst.get());
    }
}
```

**File I/O with OpenImageIO:**

```cpp
std::string ComfyUIBasePlugin::writeInputImage(OFX::Image* img, int frame) {
    using namespace OIIO;

    // Get image properties
    OfxRectI bounds = img->getBounds();
    int width = bounds.x2 - bounds.x1;
    int height = bounds.y2 - bounds.y1;
    OFX::BitDepthEnum bitDepth = img->getPixelDepth();

    // Build output path
    std::string projectName = _projectName->getValue();
    std::string filename = formatString("input_%04d.exr", frame);
    std::string fullPath = _sharedMountPath->getValue() + "/in/" +
                          projectName + "/segmentation/" + filename;

    // Create output spec
    ImageSpec spec(width, height, 4, TypeDesc::FLOAT);
    spec.attribute("compression", "zip");

    // Create output
    auto out = ImageOutput::create(fullPath);
    if (!out) {
        throw std::runtime_error("Could not create output image");
    }

    out->open(fullPath, spec);

    // Convert OFX image data to float buffer
    std::vector<float> buffer(width * height * 4);
    convertOFXToFloat(img, buffer.data());

    // Write scanlines
    out->write_image(TypeDesc::FLOAT, buffer.data());
    out->close();

    return fullPath;
}
```

**WebSocket Progress Monitoring:**

```cpp
void ComfyUIBasePlugin::waitForCompletion(const std::string& promptId) {
    bool completed = false;
    float progress = 0.0f;

    _comfyClient->monitorExecution(promptId,
        [&](const std::string& nodeName, float nodeProgress) {
            // Update progress
            progress = nodeProgress;
            reportProgress(progress, "Processing: " + nodeName);

            // Check for user abort
            if (abort()) {
                _comfyClient->interruptExecution(_comfyClient->getClientId());
                throw std::runtime_error("User cancelled");
            }
        }
    );
}
```

#### 1.3 Parameter System Mapping

**Python UI → OFX Parameters:**

| Python Widget | OFX Parameter Type | Example |
|--------------|-------------------|---------|
| String input | `OFX::StringParam` | Server address |
| Integer input | `OFX::IntParam` | Port, resolution |
| Float input | `OFX::DoubleParam` | Threshold |
| Dropdown | `OFX::ChoiceParam` | Model selection |
| Checkbox | `OFX::BooleanParam` | Enable cache |
| Button | `OFX::PushButtonParam` | Refresh models |
| Color picker | `OFX::RGBAParam` | Color parameters |
| Text area | `OFX::StringParam` | Text prompt |

**Parameter Definition Example:**

```cpp
void ComfyUIBasePlugin::defineParameters(OFX::ImageEffectDescriptor &desc) {
    // Server configuration group
    OFX::GroupParamDescriptor *serverGroup = desc.defineGroupParam("serverGroup");
    serverGroup->setLabel("ComfyUI Server");
    serverGroup->setHint("Configuration for ComfyUI REST server connection");

    OFX::StringParamDescriptor *serverAddr = desc.defineStringParam("serverAddress");
    serverAddr->setLabel("Server Address");
    serverAddr->setHint("Hostname or IP address of ComfyUI server");
    serverAddr->setDefault("localhost");
    serverAddr->setParent(*serverGroup);

    OFX::IntParamDescriptor *serverPort = desc.defineIntParam("serverPort");
    serverPort->setLabel("Port");
    serverPort->setHint("ComfyUI server port number");
    serverPort->setDefault(8188);
    serverPort->setRange(1, 65535);
    serverPort->setDisplayRange(8000, 9000);
    serverPort->setParent(*serverGroup);

    // Shared storage group
    OFX::GroupParamDescriptor *storageGroup = desc.defineGroupParam("storageGroup");
    storageGroup->setLabel("Shared Storage");

    OFX::StringParamDescriptor *mountPath = desc.defineStringParam("sharedMountPath");
    mountPath->setLabel("Mount Path");
    mountPath->setHint("Path to shared network storage for image exchange");
    mountPath->setStringType(OFX::eStringTypeDirectoryPath);
    mountPath->setParent(*storageGroup);

    OFX::StringParamDescriptor *projectName = desc.defineStringParam("projectName");
    projectName->setLabel("Project Name");
    projectName->setHint("Project identifier for organizing files");
    projectName->setDefault("flame_project");
    projectName->setParent(*storageGroup);
}
```

---

### Phase 2: Segmentation Plugin Implementation

#### 2.1 Plugin Class Definition

```cpp
class ComfyUISegmentationPlugin : public ComfyUIBasePlugin {
private:
    // Segmentation-specific parameters
    OFX::ChoiceParam *_samModel;
    OFX::ChoiceParam *_dinoModel;
    OFX::DoubleParam *_threshold;
    OFX::IntParam *_resolution;
    OFX::StringParam *_prompt;
    OFX::BooleanParam *_outputMatte;

    // Model lists
    std::vector<std::string> _availableSAMModels;
    std::vector<std::string> _availableDINOModels;

    // Workflow template
    json _workflowTemplate;

public:
    ComfyUISegmentationPlugin(OfxImageEffectHandle handle);

    // Implement abstract methods
    virtual json buildWorkflow() override;
    virtual void setupParameters(OFX::ImageEffectDescriptor &desc,
                                OFX::ContextEnum context) override;
    virtual std::vector<std::string> getRequiredModels() override;
    virtual void processOutput(const std::vector<ImageData>& results) override;

    // Segmentation-specific
    void refreshModelLists();
    json loadWorkflowTemplate();
};
```

#### 2.2 Workflow Building

```cpp
json ComfyUISegmentationPlugin::buildWorkflow() {
    // Load template from embedded resource
    if (_workflowTemplate.empty()) {
        _workflowTemplate = loadWorkflowTemplate();
    }

    // Clone template
    json workflow = _workflowTemplate;

    // Update model selections
    int samModelIdx = _samModel->getValue();
    int dinoModelIdx = _dinoModel->getValue();
    workflow["18"]["inputs"]["model_name"] = _availableSAMModels[samModelIdx];
    workflow["17"]["inputs"]["model_name"] = _availableDINOModels[dinoModelIdx];

    // Update processing parameters
    workflow["16"]["inputs"]["prompt"] = _prompt->getValue();
    workflow["16"]["inputs"]["threshold"] = _threshold->getValue();
    workflow["24"]["inputs"]["resolution"] = _resolution->getValue();

    // Update I/O paths
    std::string projectName = _projectName->getValue();
    std::string inputPath = formatInputPath(projectName);
    std::string outputPath = formatOutputPath(projectName);

    workflow["1"]["inputs"]["image"] = inputPath;
    workflow["27"]["inputs"]["filename_prefix"] = outputPath + "/Result";
    workflow["23"]["inputs"]["filename_prefix"] = outputPath + "/OutMatte";

    return workflow;
}
```

#### 2.3 Parameter Setup

```cpp
void ComfyUISegmentationPlugin::setupParameters(OFX::ImageEffectDescriptor &desc,
                                               OFX::ContextEnum context) {
    // Call base class
    ComfyUIBasePlugin::setupParameters(desc, context);

    // Model selection group
    OFX::GroupParamDescriptor *modelGroup = desc.defineGroupParam("modelGroup");
    modelGroup->setLabel("AI Models");

    OFX::ChoiceParamDescriptor *samModel = desc.defineChoiceParam("samModel");
    samModel->setLabel("SAM Model");
    samModel->setHint("Select Segment Anything Model version");
    // Options populated at runtime via refreshModelLists()
    samModel->appendOption("sam_vit_h (2.56GB)");
    samModel->appendOption("sam_vit_l (1.25GB)");
    samModel->appendOption("sam_vit_b (375MB)");
    samModel->setDefault(0);
    samModel->setParent(*modelGroup);

    OFX::ChoiceParamDescriptor *dinoModel = desc.defineChoiceParam("dinoModel");
    dinoModel->setLabel("GroundingDINO Model");
    dinoModel->setHint("Select object detection model");
    dinoModel->appendOption("GroundingDINO_SwinT_OGC (694MB)");
    dinoModel->appendOption("GroundingDINO_SwinB (938MB)");
    dinoModel->setDefault(0);
    dinoModel->setParent(*modelGroup);

    // Processing parameters group
    OFX::GroupParamDescriptor *procGroup = desc.defineGroupParam("processingGroup");
    procGroup->setLabel("Segmentation Parameters");

    OFX::StringParamDescriptor *prompt = desc.defineStringParam("prompt");
    prompt->setLabel("Prompt");
    prompt->setHint("Natural language description of object to segment");
    prompt->setDefault("foreground");
    prompt->setParent(*procGroup);

    OFX::DoubleParamDescriptor *threshold = desc.defineDoubleParam("threshold");
    threshold->setLabel("Threshold");
    threshold->setHint("Detection confidence threshold (0.0-1.0)");
    threshold->setDefault(0.3);
    threshold->setRange(0.0, 1.0);
    threshold->setDisplayRange(0.0, 1.0);
    threshold->setIncrement(0.01);
    threshold->setDigits(3);
    threshold->setParent(*procGroup);

    OFX::IntParamDescriptor *resolution = desc.defineIntParam("resolution");
    resolution->setLabel("Resolution");
    resolution->setHint("Processing resolution for SAM preprocessor");
    resolution->setDefault(1080);
    resolution->setRange(256, 4096);
    resolution->setDisplayRange(512, 2160);
    resolution->setParent(*procGroup);

    // Output options
    OFX::BooleanParamDescriptor *outputMatte = desc.defineBooleanParam("outputMatte");
    outputMatte->setLabel("Output Matte");
    outputMatte->setHint("Enable separate matte output clip");
    outputMatte->setDefault(true);
}
```

#### 2.4 Multi-Output Handling

**Approach 1: Multiple Clips (if host supports)**

```cpp
void ComfyUISegmentationPluginFactory::describeInContext(
    OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context) {

    // Input clip
    OFX::ClipDescriptor *srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->setSupportsTiles(true);

    // Main output (segmented result)
    OFX::ClipDescriptor *dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->setSupportsTiles(true);

    // Optional: Matte output clip (if host supports multiple outputs)
    OFX::ClipDescriptor *matteClip = desc.defineClip("Matte");
    matteClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    matteClip->setOptional(true);
    matteClip->setSupportsTiles(true);
}
```

**Approach 2: Embedded Matte (universal compatibility)**

```cpp
void ComfyUISegmentationPlugin::processOutput(const std::vector<ImageData>& results) {
    // results[0] = colored segmentation result
    // results[1] = black & white matte

    std::unique_ptr<OFX::Image> dst(_dstClip->fetchImage(args.time));

    if (_outputMatte->getValue()) {
        // Embed matte in alpha channel
        copyRGBFromResult(results[0], dst.get());
        copyAlphaFromMatte(results[1], dst.get());
    } else {
        // Copy full RGBA from result
        copyFullImage(results[0], dst.get());
    }
}
```

---

### Phase 3: Build System Integration

#### 3.1 CMakeLists.txt

```cmake
# Segmentation plugin
add_library(ComfyUISegmentation MODULE
    segmentation/comfyui_segmentation.cpp
    segmentation/comfyui_base.cpp
    segmentation/comfyui_client.cpp
)

target_include_directories(ComfyUISegmentation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/segmentation
    ${CMAKE_SOURCE_DIR}/Support/include
)

# Dependencies via Conan
find_package(OpenImageIO REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(httplib REQUIRED)
find_package(websocketpp REQUIRED)

target_link_libraries(ComfyUISegmentation PRIVATE
    OFX::Support
    OpenImageIO::OpenImageIO
    nlohmann_json::nlohmann_json
    httplib::httplib
    websocketpp::websocketpp
)

# Set bundle properties
set_target_properties(ComfyUISegmentation PROPERTIES
    BUNDLE TRUE
    BUNDLE_EXTENSION ofx.bundle
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.reepost.comfyui.segmentation"
    MACOSX_BUNDLE_BUNDLE_NAME "ComfyUI Segmentation"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
)

# Install to OFX plugin directory
install(TARGETS ComfyUISegmentation
    BUNDLE DESTINATION "$ENV{HOME}/Library/OFX/Plugins"
    COMPONENT plugins
)
```

#### 3.2 Conan Dependencies

```python
[requires]
openimageio/2.4.14.0
nlohmann_json/3.11.2
cpp-httplib/0.14.1
websocketpp/0.8.2
openssl/3.1.3  # For HTTPS support
zlib/1.3       # For EXR compression

[generators]
CMakeDeps
CMakeToolchain

[options]
openimageio:with_openexr=True
openimageio:with_freetype=False
```

---

### Phase 4: Advanced Features

#### 4.1 Caching System

**Problem:** Avoid re-running ComfyUI for unchanged inputs

**Solution:**

```cpp
class CacheManager {
public:
    struct CacheKey {
        std::string inputHash;
        json workflowHash;

        bool operator==(const CacheKey& other) const {
            return inputHash == other.inputHash &&
                   workflowHash == other.workflowHash;
        }
    };

    CacheKey computeKey(OFX::Image* input, const json& workflow);
    bool hasCache(const CacheKey& key);
    std::vector<ImageData> retrieveCache(const CacheKey& key);
    void storeCache(const CacheKey& key, const std::vector<ImageData>& results);

private:
    std::string _cacheDirectory;
    std::unordered_map<CacheKey, std::vector<std::string>> _cacheIndex;
};

// In render()
CacheKey key = _cacheManager->computeKey(src.get(), workflow);
if (_enableCache->getValue() && _cacheManager->hasCache(key)) {
    // Use cached results
    auto results = _cacheManager->retrieveCache(key);
    processOutput(results);
    return;
}
// Otherwise, run ComfyUI workflow...
```

#### 4.2 Asynchronous Processing

**Problem:** Long-running workflows block host UI

**Solution:**

```cpp
class AsyncExecutor {
public:
    using CompletionCallback = std::function<void(const std::vector<ImageData>&)>;

    void submitWorkflow(const json& workflow, CompletionCallback callback);
    bool isComplete(const std::string& promptId);
    void cancel(const std::string& promptId);

private:
    std::thread _workerThread;
    std::queue<WorkItem> _workQueue;
    std::mutex _queueMutex;
    std::condition_variable _queueCV;
};

// In render() for preview/interactive mode
if (_processingMode->getValue() == ASYNC) {
    _asyncExecutor->submitWorkflow(workflow, [this](const auto& results) {
        // Update cached result
        _cachedResult = results;
        // Notify host to refresh
        this->redrawOverlays();
    });

    // Return cached result immediately
    if (_cachedResult.empty()) {
        // First frame - show "processing" message
        renderProcessingMessage(dst.get());
    } else {
        processOutput(_cachedResult);
    }
}
```

#### 4.3 Progress Reporting

**OFX Progress Suite Integration:**

```cpp
void ComfyUIBasePlugin::reportProgress(float progress, const std::string& status) {
    OfxProgressSuiteV1 *progressSuite =
        (OfxProgressSuiteV1*)getHost()->fetchSuite(kOfxProgressSuite, 1);

    if (progressSuite) {
        progressSuite->progressUpdate(getHandle(), progress);
    }

    // Optional: Log to host console
    OFX::Log::print("ComfyUI: %s (%.1f%%)", status.c_str(), progress * 100);
}
```

#### 4.4 Error Handling and Reporting

```cpp
void ComfyUIBasePlugin::handleComfyError(const std::string& error) {
    // Parse ComfyUI error response
    json errorData = json::parse(error);

    std::string errorMsg = "ComfyUI Error:\n";

    if (errorData.contains("node_errors")) {
        for (const auto& [nodeId, nodeError] : errorData["node_errors"].items()) {
            errorMsg += formatString("Node %s: %s\n",
                                    nodeId.c_str(),
                                    nodeError["message"].get<std::string>().c_str());
        }
    }

    // Report to host
    OFX::Message::message(getHandle(),
                         kOfxMessageError,
                         "ComfyUI Processing Failed",
                         errorMsg.c_str());

    // Log details
    OFX::Log::error("ComfyUI Error Details:\n%s", error.c_str());

    // Update UI state
    _state = ProcessingState::Error;
}
```

#### 4.5 Model Validation

```cpp
bool ComfyUIBasePlugin::validateModelAvailability() {
    std::vector<std::string> requiredModels = getRequiredModels();
    std::vector<std::string> missingModels;

    for (const auto& model : requiredModels) {
        if (!_comfyClient->modelExists(model)) {
            missingModels.push_back(model);
        }
    }

    if (!missingModels.empty()) {
        std::string errorMsg = "Missing required models:\n";
        for (const auto& model : missingModels) {
            errorMsg += "  - " + model + "\n";
        }
        errorMsg += "\nPlease install these models on the ComfyUI server.";

        OFX::Message::message(getHandle(),
                             kOfxMessageWarning,
                             "Missing Models",
                             errorMsg.c_str());
        return false;
    }

    return true;
}
```

---

### Phase 5: Testing and Deployment

#### 5.1 Testing Strategy

**Unit Tests:**

```cpp
TEST(ComfyUIClient, ConnectionTest) {
    ComfyUIClient client("localhost:8188");
    ASSERT_TRUE(client.testConnection());
}

TEST(ComfyUIClient, QueuePrompt) {
    ComfyUIClient client("localhost:8188");
    json workflow = loadTestWorkflow();

    std::string promptId = client.queuePrompt(workflow, "test_client");
    ASSERT_FALSE(promptId.empty());
}

TEST(CacheManager, KeyComputation) {
    CacheManager cache;
    OFX::Image* img = createTestImage();
    json workflow = loadTestWorkflow();

    auto key1 = cache.computeKey(img, workflow);
    auto key2 = cache.computeKey(img, workflow);

    ASSERT_EQ(key1, key2);
}
```

**Integration Tests:**

```cpp
TEST(SegmentationPlugin, RenderTest) {
    // Set up test host environment
    TestHost host;

    // Create plugin instance
    auto plugin = createSegmentationPlugin();

    // Configure parameters
    plugin->setParameter("serverAddress", "localhost");
    plugin->setParameter("serverPort", 8188);
    plugin->setParameter("prompt", "foreground");

    // Render test frame
    OFX::Image* input = loadTestImage("test_input.exr");
    OFX::RenderArguments args;
    args.time = 0;
    args.renderWindow = input->getBounds();

    plugin->render(args);

    // Verify output
    OFX::Image* output = plugin->getOutputImage();
    ASSERT_TRUE(output != nullptr);
    ASSERT_TRUE(validateSegmentation(input, output));
}
```

**Host Compatibility Tests:**

- Flame 2024/2025
- Nuke 14/15
- DaVinci Resolve 18
- After Effects (via AE OFX wrapper)

#### 5.2 Build and Deployment

**Universal Binary (macOS):**

```bash
# Build for both architectures
cmake --preset conan-release \
      -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
      -DBUILD_COMFYUI_PLUGINS=TRUE

cmake --build build/Release --config Release --parallel

# Result: ComfyUISegmentation.ofx.bundle (universal)
file build/Release/bin/ComfyUISegmentation.ofx.bundle/Contents/MacOS/ComfyUISegmentation
# Output: Mach-O universal binary with 2 architectures: [x86_64:...] [arm64:...]
```

**Linux Distribution:**

```bash
# Build with -fvisibility=hidden for clean symbol export
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fvisibility=hidden" \
      .

make -j$(nproc)

# Result: ComfyUISegmentation.ofx.bundle/Contents/Linux-x86-64/
```

**Windows Distribution:**

```bash
# Build with MSVC
cmake -G "Visual Studio 17 2022" -A x64 .
cmake --build . --config Release

# Result: ComfyUISegmentation.ofx.bundle/Contents/Win64/
```

#### 5.3 Installation Package

**macOS .pkg Installer:**

```xml
<!-- Distribution script -->
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>ComfyUI OFX Plugins</title>
    <organization>com.reepost</organization>
    <pkg-ref id="com.reepost.comfyui.segmentation"/>
    <options customize="never" require-scripts="false"/>
    <domains enable_localSystem="true"/>

    <installation-check script="installCheck()"/>
    <script>
    function installCheck() {
        // Check for ComfyUI server availability
        // Warn if server not reachable
        return true;
    }
    </script>

    <choice id="segmentation" visible="true" title="Segmentation Plugin">
        <pkg-ref idref="com.reepost.comfyui.segmentation"/>
    </choice>
</installer-gui-script>
```

---

## Feature Comparison Matrix

| Feature | Pybox (Python) | OFX Plugin (C++) | Notes |
|---------|----------------|------------------|-------|
| **Performance** | | | |
| Processing Speed | Moderate | Fast | Native C++ execution |
| Startup Time | Slow (Python) | Fast | Compiled binary |
| Memory Usage | High | Low | Efficient memory management |
| **Compatibility** | | | |
| Host Support | Flame/Flare only | All OFX hosts | Universal compatibility |
| Platform Support | Linux/macOS | Linux/macOS/Win | Cross-platform |
| **Functionality** | | | |
| ComfyUI Integration | ✓ | ✓ | REST API + WebSocket |
| Model Management | ✓ | ✓ | Dynamic discovery |
| Workflow Execution | ✓ | ✓ | JSON-based workflows |
| Multi-output | ✓ | ✓ | Result + Matte |
| Caching | Limited | Advanced | Hash-based caching |
| Progress Reporting | Basic | Full | OFX progress suite |
| Error Handling | Basic | Robust | Detailed error reporting |
| **Deployment** | | | |
| Installation | Python deps | Single bundle | Simplified deployment |
| Updates | Manual | Package manager | Easier updates |
| Dependencies | Many | Bundled | Self-contained |

---

## Development Roadmap

### Milestone 1: Foundation (Weeks 1-4)

- [ ] Implement ComfyUI C++ REST client
- [ ] WebSocket integration for status monitoring
- [ ] Image I/O with OpenImageIO/EXR support
- [ ] Base plugin class architecture
- [ ] Parameter system implementation
- [ ] Unit tests for core components

### Milestone 2: Segmentation Plugin (Weeks 5-8)

- [ ] Implement segmentation-specific workflow building
- [ ] Model discovery and validation
- [ ] Multi-output handling (result + matte)
- [ ] Parameter UI implementation
- [ ] Integration testing with ComfyUI server
- [ ] Error handling and user feedback

### Milestone 3: Advanced Features (Weeks 9-12)

- [ ] Implement caching system
- [ ] Asynchronous processing mode
- [ ] Progress reporting integration
- [ ] Model download/update mechanism
- [ ] Performance optimization
- [ ] Memory management refinement

### Milestone 4: Testing and Polish (Weeks 13-16)

- [ ] Comprehensive host compatibility testing
- [ ] Performance benchmarking
- [ ] Documentation completion
- [ ] User guide and tutorials
- [ ] Build universal binaries
- [ ] Create installation packages

### Milestone 5: Additional Nodes (Weeks 17+)

- [ ] Upscaling plugin (Real-ESRGAN)
- [ ] Inpainting plugin (Stable Diffusion)
- [ ] Style transfer plugin
- [ ] Depth estimation plugin
- [ ] [Additional AI workflows as needed]

---

## Best Practices and Recommendations

### Code Organization

```
ComfyUIPlugins/
├── CMakeLists.txt
├── conanfile.py
├── common/
│   ├── comfyui_client.h
│   ├── comfyui_client.cpp
│   ├── comfyui_base_plugin.h
│   ├── comfyui_base_plugin.cpp
│   ├── cache_manager.h
│   ├── cache_manager.cpp
│   └── utils.h
├── segmentation/
│   ├── segmentation_plugin.h
│   ├── segmentation_plugin.cpp
│   ├── workflow_template.json
│   └── CMakeLists.txt
├── upscale/
│   ├── upscale_plugin.h
│   ├── upscale_plugin.cpp
│   └── workflow_template.json
├── tests/
│   ├── client_tests.cpp
│   ├── plugin_tests.cpp
│   └── integration_tests.cpp
└── docs/
    ├── API.md
    ├── BUILDING.md
    └── USER_GUIDE.md
```

### Error Handling Strategy

1. **Never crash** - Catch all exceptions and report via OFX error system
2. **Validate early** - Check server connection and model availability before rendering
3. **User-friendly messages** - Provide actionable error messages
4. **Graceful degradation** - Fall back to safe defaults when possible
5. **Detailed logging** - Log technical details for debugging

### Performance Optimization

1. **Lazy initialization** - Connect to server only when needed
2. **Connection pooling** - Reuse HTTP connections
3. **Efficient I/O** - Use memory-mapped files for large images
4. **Multi-threading** - Process tiles in parallel where possible
5. **Smart caching** - Cache results aggressively with proper invalidation

### Security Considerations

1. **Input validation** - Sanitize all user inputs
2. **Network security** - Support HTTPS for production deployments
3. **Path traversal** - Validate all file paths
4. **Resource limits** - Implement timeouts and memory limits
5. **Credentials** - Support secure credential storage

---

## Conclusion

This transposition plan provides a comprehensive roadmap for converting the Flame/Flare Pybox ComfyUI system into professional-grade OpenFX plugins. The resulting plugins will offer:

1. **Superior Performance** - Native C++ execution
2. **Broad Compatibility** - Works in all OFX hosts
3. **Professional Quality** - Robust error handling and progress reporting
4. **Easy Deployment** - Single binary bundle with bundled dependencies
5. **Extensibility** - Clean architecture for adding new AI workflows

The base architecture follows proven OFX patterns from the OpenFX repository while incorporating modern C++ best practices. The plugin system is designed to be maintainable, testable, and extensible for future AI model integrations.

**Next Steps:**

1. Review and validate this transposition plan
2. Set up development environment and dependencies
3. Begin implementation with Milestone 1 (Foundation)
4. Establish CI/CD pipeline for automated builds and tests
5. Create documentation and user guides

---

## References

### Documentation

- OpenFX Specification: <https://openeffects.org/documentation/>
- OpenImageIO: <https://openimageio.readthedocs.io/>
- ComfyUI API: <https://github.com/comfyanonymous/ComfyUI>
- Segment Anything: <https://github.com/facebookresearch/segment-anything>

### Source Repositories

- OpenFX: <https://github.com/AcademySoftwareFoundation/openfx>
- ComfyUI Client: <https://github.com/Dev-Reepost/flame_comfyui_client>
- Pybox Base: <https://github.com/Dev-Reepost/flame_comfyui_pybox>
- Segmentation: <https://github.com/Dev-Reepost/flame_comfyui_segmentation>

### Dependencies

- cpp-httplib: <https://github.com/yhirose/cpp-httplib>
- WebSocket++: <https://github.com/zaphoyd/websocketpp>
- nlohmann/json: <https://github.com/nlohmann/json>
- OpenImageIO: <https://github.com/AcademySoftwareFoundation/OpenImageIO>
