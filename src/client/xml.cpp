#include "xml.h"

void WriteMessage(const char* filename)
{
    // Make xml: <?xml ..><Hello>World</Hello>
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "Hello" );
    TiXmlText * text = new TiXmlText( "World" );
    element->LinkEndChild( text );
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    //elem->SetAttribute( "priority", 2 );
    //elem = elem->NextSiblingElement();

    doc.SaveFile(filename);
}

string ReadMessage(TiXmlDocument &doc)
{
    string content;
    string name;
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return "";
    }
    
    for(TiXmlElement* elem = root->FirstChildElement();
        elem != NULL;
        elem = elem->NextSiblingElement())
    {
        const char* attr;
        string elemName = elem->Value();
        //cout << "elem: " << elemName << endl;
        if(elemName == "name")
        {
            attr = elem->Attribute("level");
            if(attr != NULL);
        }
        
        else if(elemName == "content")
        {
            attr = elem->Attribute("type");
            if(attr != NULL);
        }
        
        for(TiXmlNode* e = elem->FirstChild(); e != NULL; e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "name")
            {
                name = text->Value();
            }
            else if (elemName == "content")
            {
                content = text->Value();
            }
        }
     }

     stringstream ss;
     ss << "<" << name << "> " << content << std::  endl;
     return ss.str();
}
