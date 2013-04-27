#include "xml-message.h"
#include "crypto/base64.h"

std::string XMLBuildMessage(const char* name, const char* content)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "message" );
    
    TiXmlElement * element_name = new TiXmlElement( "name" );
    TiXmlElement * element_content = new TiXmlElement( "content" );
        
    TiXmlText * text_name = new TiXmlText( EncodeBase64(name) );
    TiXmlText * text_content = new TiXmlText( EncodeBase64(content) );
    
    element->LinkEndChild( element_name );
    element->LinkEndChild( element_content );
    
    element_name->LinkEndChild( text_name );
    element_content->LinkEndChild( text_content );
    
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    stringstream ss;
    ss << doc;
 
    std::string sss = XMLBuildUpdate("alec", "join");
    std::string nx,sx;
    XMLReadUpdate(sss.c_str(), nx,sx);
    INFO("debug", "XML: %s - %s #\n", nx.c_str(),sx.c_str());
 
    return ss.str(); 
}

void XMLReadMessage(const char *str, string& name, string& content)
{
    if(!str)
        return;
        
    TiXmlDocument doc;
    doc.Parse(str);
    
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return;
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
        
        for(TiXmlNode* e = elem->FirstChild();
            e != NULL;
            e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "name")
            {
                name = DecodeBase64(text->Value());
            }
            else if (elemName == "content")
            {
                content = DecodeBase64(text->Value());
            }
        }
     }
}

std::string XMLBuildUpdate(const char* name, const char* status)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "update" );
    
    TiXmlElement * element_name = new TiXmlElement( "name" );
    TiXmlText * text_name = new TiXmlText( EncodeBase64(name) );

    element->LinkEndChild( element_name );
    element_name->LinkEndChild( text_name );
    
    element_name->SetAttribute("status", EncodeBase64(status));
    
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    stringstream ss;
    ss << doc;

    INFO("debug","XML: %s\n", ss.str().c_str());
     
    return ss.str(); 
}

void XMLReadUpdate(const char *str, string& name, string& status)
{
    if(!str)
        return;
        
    TiXmlDocument doc;
    doc.Parse(str);
    
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return;
    }
    
    for(TiXmlElement* elem = root->FirstChildElement();
        elem != NULL;
        elem = elem->NextSiblingElement())
    {
        const char* attr;
        string elemName = elem->Value();
        if(elemName == "name")
        {
            attr = elem->Attribute("status");
            if(attr != NULL)
            {
                status = DecodeBase64(attr);
            }
        }
        
        for(TiXmlNode* e = elem->FirstChild();
            e != NULL;
            e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "name")
            {
                name = DecodeBase64(text->Value());
            }
        }
     }
}
