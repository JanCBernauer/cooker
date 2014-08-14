
#include "InitReader.h"


#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xqilla/xqilla-dom3.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <sys/time.h>
#include <cstdio>


char commanddate[]="declare variable $date as xs:dateTime external;\n\
 declare variable $detector as xs:string external;\n\
 for $i in     for $h in /cookerinit/*[name()=$detector]/date/*\
 let $n:=name($h)\n\
 where $h/../@time<=$date\n\
   return \n\
       	   	      (: in case of IDs, flatten hierachy :)\n\
	       	      if ( $h/id ) then \n\
	       	      	 for $k in $h/id\n\
		      	     return element{node-name ($h)}\n\
		      	       { $h/../@time, $h/@* ,$k/@id, attribute {\"fct\"}{data(/cookerinit/*[name()=$detector]/config/*[name()=$n]) },$k/node()}\n\
		      else\n\
			    element{node-name ($h)}\n\
		       	       { $h/../@time, $h/@* , attribute {\"fct\"}{data(/cookerinit/*[name()=$detector]/config/*[name()=$n]) },$h/node()}\n\
		 	       (: Attach time to child :)\n\
    order by name($i),$i/@id, $i/@time descending\n\
    return $i";

char commandrun[]="declare variable $runnum as xs:integer external;\n\
 declare variable $detector as xs:string external;\n\
 for $i in     for $h in /cookerinit/*[name()=$detector]/run/*\
 let $n:=name($h)\n\
 where $h/../@nr<=$runnum\n\
   return \n\
       	   	      (: in case of IDs, flatten hierachy :)\n\
	       	      if ( $h/id ) then \n\
	       	      	 for $k in $h/id\n\
		      	     return element{node-name ($h)}\n\
		      	       { $h/../@nr, $h/@* ,$k/@id, attribute {\"fct\"}{data(/cookerinit/*[name()=$detector]/config/*[name()=$n]) },$k/node()}\n\
		      else\n\
			    element{node-name ($h)}\n\
		       	       { $h/../@nr, $h/@* , attribute {\"fct\"}{data(/cookerinit/*[name()=$detector]/config/*[name()=$n]) },$h/node()}\n\
		 	       (: Attach time to child :)\n\
    order by name($i),$i/@id, $i/@nr descending\n\
    return $i";

 
XERCES_CPP_NAMESPACE_USE;

class xmlbasefilter: public DOMLSParserFilter::DOMLSParserFilter
{
public:
  virtual FilterAction startElement(DOMElement * node )
  {
    if (std::string(UTF8(node->getAttribute(X("xml:base"))))=="homedir/.cooker/shared/")
      {
	char buf[10000];
	snprintf(buf,10000,"file://%s/.cooker/shared/",getenv("COOKERHOME"));
	node->setAttribute(X("xml:base"),X(buf));
      }
  return FILTER_ACCEPT ;
  }
  virtual FilterAction acceptNode(DOMNode*)
  {
    return FILTER_ACCEPT ;
  }

  virtual long unsigned int getWhatToShow() const {
    return DOMNodeFilter::SHOW_ALL;
  } ;
 
};



// return  $nodes[$i][ name($nodes[($i)-1])!=name($nodes[$i]) or $nodes[($i)-1]/@id!=$nodes[$i]/@id ]";			
class ErrorHandler:DOMErrorHandler
{

public:
  virtual bool handleError(const DOMError & domError)
  {
    DOMLocator *l=domError.getLocation();
    std::string near="(unknown)";
    int sev=domError.getSeverity();
    std::string sever;
    switch (sev)
      {
      case DOMError::DOM_SEVERITY_ERROR:
	sever="Error";
	break;
      case DOMError::DOM_SEVERITY_WARNING:
	sever="Warning";
	break;
      case DOMError::DOM_SEVERITY_FATAL_ERROR:
	sever="Fatal Error";
      }

    if (l->getRelatedNode()) near=UTF8(l->getRelatedNode()->getNodeName());
    //ignore Non Fatal for now.
    if (sev==DOMError::DOM_SEVERITY_FATAL_ERROR)
      {
	std::cerr<<"Init file parsing error, Severity:"<<sever<<"\n  File:"<<UTF8(l->getURI())<<"\n  Message:"<<UTF8(domError.getMessage())<<"\n  at Line number:"<<l->getLineNumber()<<" Column:"<<l->getColumnNumber()<<" Near:"<<near<<std::endl;
	if(near=="xi:include")
	  {
	    DOMElement *par=l->getRelatedNode()->getOwnerDocument()->getDocumentElement();
	    std::string xincfn=std::string(UTF8(par->getAttribute(X("xml:base"))))+UTF8(l->getRelatedNode()->getAttributes()->getNamedItem(X("href"))->getNodeValue());
	    std::cerr<<"This is an xinclude error. Reparsing included file: "<<xincfn<<std::endl;
	        DOMImplementation *xqillaImplementation =DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
    
		AutoRelease<DOMLSParser> parser(xqillaImplementation->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0));
		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
		parser->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
		parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, true);
		parser->getDomConfig()->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
		parser->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, true);
		
		if( parser->getDomConfig()->canSetParameter(XMLUni::fgXercesDoXInclude, true)){
		  parser->getDomConfig()->setParameter(XMLUni::fgXercesDoXInclude, true);
		}
		
		
		parser->getDomConfig()->setParameter(X("error-handler"),this);
		
		// Parse a DOMDocument
		DOMDocument *document = parser->parseURI(xincfn.c_str());
	  }
	
	std::cerr<<"Init File Parsing error occured... bailing out."<<std::endl;
	exit(-101);}
    return true;
}
};

InitReader::InitReader(std::string filename,std::string date, int runnr)
{
  try{ 

    XQillaPlatformUtils::initialize();
    DOMImplementation *xqillaImplementation =DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
    
    AutoRelease<DOMLSParser> parser(xqillaImplementation->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0));
    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    parser->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, true);
    parser->getDomConfig()->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
    parser->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, true);

    if( parser->getDomConfig()->canSetParameter(XMLUni::fgXercesDoXInclude, true)){
       parser->getDomConfig()->setParameter(XMLUni::fgXercesDoXInclude, true);
    }



    ErrorHandler eh;
    parser->getDomConfig()->setParameter(X("error-handler"),&eh);
    xmlbasefilter filter;
    parser->setFilter(&filter);
    // Parse a DOMDocument
    DOMDocument *document = parser->parseURI(filename.c_str());
    if(document == 0) {
      std::cerr << "InitReader: Document not found: " <<  filename<< std::endl;
      exit(-100);
    }
    
    AutoRelease<DOMLSSerializer> Serializer(xqillaImplementation->createLSSerializer());
  
    XMLCh *docu=Serializer->writeToString(document);
    context =xqilla.createContext(XQilla::XQUERY);
    querydate= xqilla.parse(X(commanddate),context);
    context =xqilla.createContext(XQilla::XQUERY);
    querynr= xqilla.parse(X(commandrun),context);

    //   query= xqilla.parse(X("for $i in /cookerinit/ToF/*\nreturn $i"),context);

    
    Item::Ptr item;
    item = context->getItemFactory()->createDateTime(X(date.c_str()), context);
    Sequence  sdate(1);
    sdate.addItem(item);
    context->setExternalVariable(X("date"), sdate);

    
    item= context->getItemFactory()->createInteger(runnr,context); 
    Sequence srunnr(1);
    srunnr.addItem(item);
    context->setExternalVariable(X("runnum"),srunnr);
    

    // Parse a document, and set it as the context item
    std::string buf=UTF8(docu);

    xercesc::MemBufInputSource mbis((const XMLByte*) buf.c_str(), buf.length(), "BUFFER");
  
    context->setContextItem(context->parseDocument(mbis));
    context->setContextPosition(1);
    context->setContextSize(1);
  }
  catch(   XQException E)
    {
      std::cerr<<"Parsing of init XML failed, error type:"<<UTF8(E.getType())<<" Error:"<<UTF8(E.getError())<<std::endl;
    } 
}


InitReader::~InitReader()
{
  delete querydate;
  delete querynr;
}



std::map<std::string,std::vector<std::string> > InitReader::getConfig(std::string name)
{
  std::string lastid="";
  std::string lastfct="";
  std::map<std::string,std::vector<std::string> > erg;
 // Execute the query, using the context
  try{
    Item::Ptr det;
    det = context->getItemFactory()->createString(X(name.c_str()), context);
    Sequence detector(1);
    detector.addItem(det);
    context->setExternalVariable(X("detector"), detector);
    Node::Ptr item;
    Result result(0);
    for (int i=0;i<2;i++)
      {
	if (i==0)
	  result = querydate->execute(context);
	else
	  result = querynr->execute(context);
	while(item = result->next(context)) {
	  Result attribs=item->dmAttributes(context,0);
	  Node::Ptr attrib;
	  std::string id="";
	  std::string fct="";
	  while(attrib=attribs->next(context))
	    {
	      std::string name=UTF8(attrib->dmNodeName(context)->getName());
	      std::string value=UTF8(attrib->dmStringValue(context));
	      
	      if (name=="id") id=value;
	      if (name=="fct"){
		//sanitize the fct name
		fct=value;
		boost::trim(fct);
		
	      }
	    }
	  std::string value=UTF8(item->dmStringValue(context));
	  if ((fct!=lastfct )||(id!=lastid))
	    {	
	      if (id=="")
		erg[fct].push_back(value);
	      else
		erg[fct].push_back(id+","+value);
	      lastid=id;
	      lastfct=fct;
	    }
	  
	}
      }
  }
  catch(   XQException E)
    {
      std::cerr<<"Querying of init XML failed, error type:"<<UTF8(E.getType())<<" Error:"<<UTF8(E.getError())<<std::endl;
    }
  return erg;
}
