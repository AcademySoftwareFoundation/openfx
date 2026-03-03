"""
Sphinx extension to add links from property documentation to structured documentation.
"""

from docutils import nodes
from docutils.parsers.rst import Directive, directives
from sphinx.util.docutils import SphinxDirective

class PropertyLinkDirective(SphinxDirective):
    """
    Custom directive to add links to structured documentation.
    """
    required_arguments = 1
    optional_arguments = 0
    has_content = False

    def run(self):
        prop_name = self.arguments[0]
        # Remove the 'k' prefix for the reference
        if prop_name.startswith('k'):
            ref_name = prop_name[1:]
        else:
            ref_name = prop_name
            
        # Create a paragraph node with the link
        para = nodes.paragraph()
        para['classes'].append('property-link')
        
        # Create a reference node
        reference = nodes.reference('', f'See structured property reference for {prop_name}')
        # Convert to lowercase for the actual HTML anchor
        reference['refuri'] = f'ofxPropertiesReferenceGenerated.html#{ref_name.lower()}'
        para += reference
        
        return [para]

def setup(app):
    app.add_directive('property_link', PropertyLinkDirective)
    
    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
