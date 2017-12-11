//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2017, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of an XML document.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsxmlNode.h"
#include "tsReport.h"

namespace ts {
    namespace xml {
        //!
        //! Representation of an XML document.
        //!
        class TSDUCKDLL Document: public Node
        {
        public:
            //!
            //! Constructor.
            //!
            Document() : Node(1) {}

            //!
            //! Parse an XML document.
            //! @param [in] lines List of text lines forming the XML document.
            //! @param [in] report Where to report errors.
            //! @return True on success, false on error.
            //!
            bool parse(const UStringList& lines, Report& report);

            //!
            //! Parse an XML document.
            //! @param [in] text The XML document.
            //! @param [in] report Where to report errors.
            //! @return True on success, false on error.
            //!
            bool parse(const UString& text, Report& report);

            //!
            //! Load and parse an XML file.
            //! @param [in] fileName The XML file name.
            //! @param [in] report Where to report errors.
            //! @return True on success, false on error.
            //!
            bool load(const UString& fileName, Report& report);

        private:
            // Unaccessible operations.
            Document(const Document&) = delete;
            Document& operator=(const Document&) = delete;
        };
    }
}
