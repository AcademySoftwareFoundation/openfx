/*
  OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
  Copyright (C) 2004-2005 The Foundry Visionmongers Ltd
  Author Bruno Nicoletti bruno@thefoundry.co.uk

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  The Foundry Visionmongers Ltd
  35-36 Gt Marlborough St 
  London W1V 7FN
  England
*/

#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

/** @file This file contains the body of functions used for logging ofx problems etc...

The log file is written to using printf style functions, rather than via c++ iostreams.

 */

#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

namespace OFX {
    namespace Log {

        /** @brief log file */
        static FILE *gLogFP = 0;
   
        /** @brief the global logfile name */
        static std::string gLogFileName("ofxTestLog.txt");

        /** @brief global indent level, not MP sane */
        static int gIndent = 0;
  
        /** @brief Sets the name of the log file. */
        void
        setFileName(const std::string &value)
        {
            gLogFileName = value;
        }
  
        /** @brief Opens the log file, returns whether this was sucessful or not. */
        bool
        open(void)
        {
#ifdef DEBUG
            if(!gLogFP) {
                gLogFP = fopen(gLogFileName.c_str(), "w");
                assert(gLogFP != 0);
            }
#endif
            return gLogFP != 0;
        }

        /** @brief Closes the log file. */
        void
        close(void)
        {
            if(gLogFP) {
                fclose(gLogFP);
            }
            gLogFP = 0;
        }

        /** @brief Indent it, not MP sane at the moment */
        void indent(void)
        {
            ++gIndent;
        }

        /** @brief Outdent it, not MP sane at the moment */
        void outdent(void)
        {
            --gIndent;
        }

        /** @brief do the indenting */
        static void doIndent(void)
        {
            if(open()) {
                for(int i = 0; i < gIndent; i++) {
                    fputs("    ", gLogFP);
                }
            }
        }

        /** @brief Prints to the log file. */
        void
        print(const char *format, ...)
        {
            if(open()) {
                doIndent();
                va_list args;
                va_start(args, format);
                vfprintf(gLogFP, format, args);
                fputc('\n', gLogFP);
                fflush(gLogFP);
                va_end(args);
            }  
        }

        /** @brief Prints to the log file only if the condition is true and prepends a warning notice. */
        void
        warning(bool condition, const char *format, ...)
        {
            if(condition && open()) {
                doIndent();
                fputs("WARNING : ", gLogFP);

                va_list args;
                va_start(args, format);
                vfprintf(gLogFP, format, args);
                fputc('\n', gLogFP);
                va_end(args);

                fflush(gLogFP);
            }  
        }

        /** @brief Prints to the log file only if the condition is true and prepends an error notice. */
        void
        error(bool condition, const char *format, ...)
        {
            if(condition && open()) {
                doIndent();
                fputs("ERROR : ", gLogFP);

                va_list args;
                va_start(args, format);
                vfprintf(gLogFP, format, args);
                fputc('\n', gLogFP);
                va_end(args);

                fflush(gLogFP);
            }  
        }
    };
};
