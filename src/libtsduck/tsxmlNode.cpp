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

#include "tsxmlNode.h"
#include "tsxmlParser.h"
#include "tsxmlDocument.h"
#include "tsxmlDeclaration.h"
#include "tsxmlElement.h"
TSDUCK_SOURCE;


//----------------------------------------------------------------------------
// Node constructor.
//----------------------------------------------------------------------------

ts::xml::Node::Node(size_t line) :
    RingNode(),
    _value(),
    _parent(this),
    _firstChild(0),
    _inputLineNum(line),
    _closingType(OPEN)
{
}


//----------------------------------------------------------------------------
// Node virtual destructor.
//----------------------------------------------------------------------------

ts::xml::Node::~Node()
{
    clear();
    reparent(0);
}


//----------------------------------------------------------------------------
// Clear the content of the node.
//----------------------------------------------------------------------------

void ts::xml::Node::clear()
{
    // Free all our children nodes.
    while (_firstChild != 0) {
        // The child will cleanly remove itself from the list of children.
        delete _firstChild;
    }

    // Clear other fields.
    _value.clear();
    _inputLineNum = 0;
}


//----------------------------------------------------------------------------
// Attach the node to a new parent.
//----------------------------------------------------------------------------

void ts::xml::Node::reparent(Node* newParent)
{
    // If the parent does not change (including zero), nothing to do.
    if (newParent == _parent) {
        return;
    }

    // Detach from our parent.
    if (_parent != 0) {
        // If we are the first child, make the parent point to the next child.
        // Unless we are alone in the ring of children, in which case the parent has no more children.
        if (_parent->_firstChild == this) {
            _parent->_firstChild = ringAlone() ? 0 : ringNext<Node>();
        }
        // Remove ourselves from our parent's children.
        ringRemove();
    }

    // Set new parent.
    _parent = newParent;

    // Insert inside new parent structure.
    if (_parent != 0) {
        if (_parent->_firstChild == 0) {
            // We become the only child of the parent.
            _parent->_firstChild = this;
        }
        else {
            // Insert in the ring of children, "before the first child", meaning at end of list.
            ringInsertBefore(_parent->_firstChild);
        }
    }
}


//----------------------------------------------------------------------------
// Get the next sibling node.
//----------------------------------------------------------------------------

ts::xml::Node* ts::xml::Node::nextSibling()
{
    // When the ring points to the first child, this is the end of the list.
    Node* next = ringNext<Node>();
    return next == this || (_parent != 0 && next == _parent->_firstChild) ? 0 : next;
}


//----------------------------------------------------------------------------
// Continue the parsing of a document from the point where this node start up
// to the end. This is the base Node implementation.
//----------------------------------------------------------------------------

bool ts::xml::Node::parseContinue(Parser& parser, UString& endToken)
{
    bool result = true;
    Node* node;

    // Loop on each token we find.
    while ((node = parser.identify()) != 0) {

        bool thisOneOk = true;

        // Read the complete node.
        if (!node->parseContinue(parser, endToken)) {
            parser.errorAtLine(node->lineNumber(), u"parsing error", {});
            thisOneOk = false;
        }
        else {
            // Special checks for a few types.
            Declaration* decl = dynamic_cast<Declaration*>(node);
            Element* elem = dynamic_cast<Element*>(node);

            if (decl != 0) {
                // This is a declaration, must be at the beginning of a document.
                if (dynamic_cast<Document*>(this) == 0) {
                    parser.errorAtLine(node->lineNumber(), u"misplaced declaration, not directly inside a document", {});
                    thisOneOk = false;
                }
                // Check that all preceding nodes are declarations as well.
                for (Node* ch = firstChild(); thisOneOk && ch != 0; ch = ch->nextSibling()) {
                    if (dynamic_cast<Declaration*>(ch) == 0) {
                        parser.errorAtLine(node->lineNumber(), u"misplaced declaration, must be at the beginning of the document", {});
                        thisOneOk = false;
                    }
                }
            }

            if (elem != 0) {
                // This is an element.

                /*@@@@@@@@
                // We read the end tag. Return it to the parent.
                if ( ele->ClosingType() == XMLElement::CLOSING ) {
                    if ( parentEndTag ) {
                        ele->_value.TransferTo( parentEndTag );
                    }
                    node->_memPool->SetTracked();   // created and then immediately deleted.
                    DeleteNode( node );
                    return p;
                }

                // Handle an end tag returned to this level.
                // And handle a bunch of annoying errors.
                bool mismatch = false;
                if ( endTag.Empty() ) {
                    if ( ele->ClosingType() == XMLElement::OPEN ) {
                        mismatch = true;
                    }
                }
                else {
                    if ( ele->ClosingType() != XMLElement::OPEN ) {
                        mismatch = true;
                    }
                    else if ( !XMLUtil::StringEqual( endTag.GetStr(), ele->Name() ) ) {
                        mismatch = true;
                    }
                }
                if ( mismatch ) {
                    _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, ele->Name(), 0, initialLineNum);
                    DeleteNode( node );
                    break;
                }
                @@@@@@@@@*/
            }
        }

        // If the child element is fine, insert it.
        if (thisOneOk) {
            node->reparent(this);
        }
        else {
            delete node;
            result = false;
        }
    }

    return result;
}
