#ifndef _ofxsLog_H_
#define _ofxsLog_H_
/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

/** @file This file contains OFX logging header code
 */
   
/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {

    /** @brief this namespace wraps up logging functionality */
    namespace Log {
        /** @brief Indent it, not MP sane at the moment */
        void indent(void);

        /** @brief Outdent it, not MP sane at the moment */
        void outdent(void);

        /** @brief Sets the name of the log file. */
        void setFileName(const std::string &value);
    
        /** @brief Opens the log file, returns whether this was sucessful or not. */
        bool open(void);

        /** @brief Closes the log file. */
        void close(void);

        /** @brief Prints to the log file. */
        void print(const char *format, ...);

        /** @brief Prints to the log file only if the condition is true and prepends a warning notice. */
        void warning(bool condition, const char *format, ...);

        /** @brief Prints to the log file only if the condition is true and prepends an error notice. */
        void error(bool condition, const char *format, ...);
    };
};

#endif
