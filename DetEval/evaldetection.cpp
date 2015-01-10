/***************************************************************************
 * evaldetection.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * Changelog:
 * 31.08.2006 cw: introduced -z feature (ignore zero rectangles).
 * 27.04.2004 cw: first version
 *
 * 1 tab = 4 spaces
 ***************************************************************************/

#define VERSION		"version 2.00, 02.03.2009."

 // C
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// C++
#include <iostream>
#include <new>
#include <map>
#include <set>

// libxml2
#ifdef HAVE_LIBXML2
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#endif

// From the CIL module
#include <CIL.h>

// From the BOXES module
#include <RectEvalList.h>
#include <BoxEvaluator.h>

#include "mex.h"

bool IgnoreZeroRects;

// The comparison function needed for the map object
struct lessthanstring
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

static void usage (char *com)
{
	cerr <<	"usage:	\n"
 		<< "   " << com << " [options]  detection-xml groundtruth-xml\n\n"
     	<< "   -p <options>   Set the evaluation parameters: \n"
        << "                  -p <a>,<b>,<c>,<d>,<e>,<f>,<g>,<h>,<i>,<j>\n"
        << "                  Default values: " << EvalParameterDefaults << "\n"
		<< "   -d <count>     How many levels in the image name path do we ignore\n"
		<< "                  (detection-xml)\n"
		<< "   -g <count>     How many levels in the image name path do we ignore\n"
		<< "                  (groundtruth-xml)\n"
		<< "   -z             Ignore detected rectangles having zero coordinates\n"
		<< "   -m             Allow mixing specified and unspecified object types\n"
		<< "   -o             Specify a comme separated list of object types which shall\n"
		<< "                  shall be loaded from the groundtruth, all other objects \n"
		<< "                  will be ignored in the groundtruth (but not in the detection\n"
		<< "                  list!). Default: all objects will be loaded.\n"
		<< "   -v             Print version string\n"
		<< endl;
}

/**********************************************************
 * Global new handler for memory management
 **********************************************************/

void globalNewHandler ()
{
	cerr << "Out Of	Memory.	Sorry.\n";
	exit (1);
}

/**********************************************************
 * The error function
 **********************************************************/

void Error (char *text)
{
	cerr << text;
}

// ********************************************************************
// A help function
// ********************************************************************

static char * getIdStr (xmlNodePtr node, char *id)
{
	char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
	if(!idstr)
	{
		cerr << "*** Syntax error: did not found property '" << id << "'\n";
		exit (1);
    }
    return idstr;
}
/*
static char * getIdStrNULLIfNotFound (xmlNodePtr node, char *id)
{
	char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
	if(!idstr)
		return NULL;
	else
    	return idstr;
}*/

static int getIdIntMinus1IfNotFound (xmlNodePtr node, char *id)
{
	char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
	if(!idstr)
		return -1;
	else
    	return atol(idstr);
}


// ********************************************************************
// Help function
// ********************************************************************

inline void EatBlanks(xmlNodePtr &x)	{
	while (xmlIsBlankNode(x))
		x=x->next;
}

// ********************************************************************
// Check the name of a node
// ********************************************************************

static bool nameIsOk (xmlNodePtr node, const char *s, bool warning)
{
	if (node==NULL)
	{
		if (warning)
			cerr << "*** Syntax error: node is null searching for " << s << endl;
		return false;
	}
	if(!node->name)
	{
		if (warning)
			cerr << "*** Syntax error: node w/o name searching for " << s << endl;
		return false;
	}
	if (xmlStrcmp(node->name,(const xmlChar *) s))
	{
       if (warning)
			cerr << "*** Syntax error: found " << node->name
            	 << " searching for " << s << endl;
       return false;
    }
    // cerr << "[" << node->name << "]";
    return true;
}


// *******************************************************************************
// Read a single Image Tag
// *******************************************************************************

static RectEvalList * readICDARXMLImageTag (xmlDocPtr doc, xmlNodePtr &imagenode,
	char *&imagename, int noIgnoreDirLevels, bool allowTypeMixing,
	set<unsigned int> *allowedObjectTypesInGT)
{
	xmlNodePtr snode, ssnode;
	RectEvalList *rl;

	snode=imagenode->children;
	EatBlanks(snode);

	rl = new RectEvalList;

	// find the sub nodes in each image node
	imagename=NULL;
	while (snode != NULL) {

		if (nameIsOk(snode, "imageName", false))
		{
			char *cp = (char *) xmlNodeListGetString(doc, snode->xmlChildrenNode, 1);

			// Ignore the first noIgnoreDirLevels levels of the path in the image name
			for (int i=0; i<noIgnoreDirLevels; ++i)
			{
				while (*cp!='\0' && *cp!='\\' && *cp!='/')
					++cp;
				if (*cp=='\0')
				{
					cerr << "Cannot ignore directory level as told per option!\n";
					cerr << "The following image name is too shallow: "
					   	 << imagename << endl;
					exit(1);
				}
				++cp;
			}
			imagename=cp;
		}

		if (nameIsOk(snode, "taggedRectangles", false))
		{
			ssnode=snode->children;
			EatBlanks(ssnode);

			while (ssnode != NULL)
			{

				if (nameIsOk(ssnode, "taggedRectangle", true))
				{
					bool doAdd=true;
					RectEval *r = new RectEval;
					r->left = atol(getIdStr(ssnode, (char *) "x"));
					r->top = atol(getIdStr(ssnode, (char *) "y"));
					r->right = atol(getIdStr(ssnode, (char *) "width")) + r->left - 1;
					r->bottom = atol(getIdStr(ssnode, (char *) "height")) + r->top - 1;

					r->objectType=getIdIntMinus1IfNotFound(ssnode, (char *) "modelType");
					if (r->objectType==-1)
						r->objectType=getIdIntMinus1IfNotFound(ssnode, (char *) "typeModel");

					if ((r->objectType!=-1) && (allowedObjectTypesInGT!=NULL))
						if (allowedObjectTypesInGT->find(r->objectType)==allowedObjectTypesInGT->end())
						{
							doAdd=false;
							cerr << "Ignoring o-type " << r->objectType << " in ground truth\n";
						}

					if (doAdd && IgnoreZeroRects)
					{
						doAdd=false;
						if (r->left>0 || r->top>0 || r->right>0 || r->bottom>0)
							doAdd=true;
					}

					if (doAdd)
					{
						rl->add (r);
						if (r->objectType==-1)
							++rl->noObjectsWithoutType;
						else
							++rl->noObjectsWithType;
					}
					else
						delete r;
				}

				ssnode = ssnode->next;
				EatBlanks(ssnode);
			}
		}

		snode = snode->next;
		EatBlanks(snode);
	}

	if (imagename==NULL) {
		cerr << "Can't find image name!!\n";
		exit (1);
	}

	if ((rl->noObjectsWithoutType>0) && (rl->noObjectsWithType>0))
	{
		if (allowTypeMixing)
		{
			cerr << "Mix of specified and unspecified object types, but this has been allowed explicitly by user action\n";
		}
		else
		{
			cerr << "The XML files contain a mix of rectangles with specified "
				"object types and unspecified object types. Normally this does not make any sens "
				"since you might want to detect specific objects, e.g. specified by a model, or "
				"generic objects (text), but not both.\n\nIf you want to proceed anyway, relaunch "
				"the application with the -m option (meaning '_M_ix object types') and pray.";
			exit (1);
		}
	}

	return rl;
}

// ***********************************************************
// Read in the XML file and store it in a map
// **********************************************************

static void readICDARXML(map<const char*, RectEvalList*, lessthanstring> &m,
			             char * filename, int noIgnoreDirLevels, bool allowTypeMixing,
			             unsigned int &noObjectsWithType, unsigned int &noObjectsWithoutType,
			             set<unsigned int> *allowedObjectTypesInGT)
{
	xmlNodePtr imagenode, root;
	xmlDocPtr doc;
	RectEvalList *rl;
	char *imagename;

	// Load and parse the document
	doc = xmlParseFile(filename);
	if (doc == NULL) {
		cerr << "Couldn't parse document " << filename << endl;
		exit (1);
	}

	// Check the document type
	root = xmlDocGetRootElement(doc);
	if (!nameIsOk(root, "tagset", true)) exit (1);
	imagenode=root->children;
    EatBlanks(imagenode);

    // Travers the image nodes
    noObjectsWithType = noObjectsWithoutType = 0;
    while (imagenode != NULL)
    {

    	if (!nameIsOk(imagenode, "image", true))
    		exit (1);

		rl=readICDARXMLImageTag(doc, imagenode, imagename, noIgnoreDirLevels, allowTypeMixing,
			allowedObjectTypesInGT);

		if (rl!=NULL)
		{
			m[imagename] = rl;
			noObjectsWithType    += rl->noObjectsWithType;
			noObjectsWithoutType += rl->noObjectsWithoutType;
		}
        imagenode = imagenode->next;
       	EatBlanks(imagenode);
	}

	// clean up
	xmlFreeDoc(doc);
}

// ***********************************************************
// The main	function
// **********************************************************

int	main (int argc,	char **argv)
{
	char *filename_det, *filename_gt;
	char *evalParameterString=EvalParameterDefaults;;
	double evalParameters[10];
	int c;
	int ignoreDetDirLevels=0,
	    ignoreGTDirLevels=0;
	unsigned int gwitht, gwithoutt, dwitht, dwithoutt;
	char *objectTypeList = NULL;
	set<unsigned int> allowedObjectTypesInGT;
	bool allowTypeMixing = false;

    RectEvalList *rl_det;
    RectEvalList *rl_gt;

    rl_det = new RectEvalList;
    for (int i=0; i<1; i++){
        RectEval *r = new RectEval;
        r->left   = 10;
        r->top    = 10;
        r->right  = 100;
        r->bottom = 100;
        r->objectType = -1;
        rl_det->add(r);
        ++rl_det->noObjectsWithoutType;
    }

    rl_gt = new RectEvalList;
    for (int i=0; i<1; i++){
        // const Rect re = new Rect(10,100,10,100);
        RectEval *r = new RectEval(10,100,10,100);
        // r->left   = 10;
        // r->top    = 10;
        // r->right  = 100;
        // r->bottom = 100;
        // r->objectType = -1;
        rl_gt->add(r);
        // ++rl_gt->noObjectsWithoutType;
    }


    parseParameterString (evalParameterString, evalParameters, EV_PARAM_COUNT, (char *) "-p");
    BoxEvaluator boxEv (rl_gt, rl_det, evalParameters);
    double scores[6];
    boxEv.matchScore(&cout, rl_gt->size(),scores);
    cout << "icdar2003 : " << scores[0] << scores[1] << scores[2] <<endl;
    cout << "deteval   : " << scores[3] << scores[4] << scores[5] <<endl;

    return 0;
}



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs>1) mexErrMsgTxt("Too many output arguments");
    if (nrhs<2) mexErrMsgTxt("Usage : DetEval(det_rects, gt_rects)");
    double *input_det = mxGetPr(prhs[0]);
    double *input_gt  = mxGetPr(prhs[1]);
    int ndet  = mxGetM(prhs[0]); // number of det_rects
    int ngt   = mxGetM(prhs[1]); // number of gt_rects
    char *evalParameterString=EvalParameterDefaults;;
    double evalParameters[10];

    RectEvalList *rl_det;
    RectEvalList *rl_gt;

    // Parse input
    rl_det = new RectEvalList;
    for(int i=0; i<ndet; i++){
        int x1 = (int) input_det[i+0*ndet];
        int y1 = (int) input_det[i+1*ndet];
        int w  = (int) input_det[i+2*ndet];
        int h  = (int) input_det[i+3*ndet];
        RectEval *r = new RectEval(y1, y1+h-1, x1, x1+w-1);
        rl_det->add(r);
    }
    rl_gt = new RectEvalList;
    for(int i=0; i<ngt; i++){
        int x1 = (int) input_gt[i+0*ngt];
        int y1 = (int) input_gt[i+1*ngt];
        int w  = (int) input_gt[i+2*ngt];
        int h  = (int) input_gt[i+3*ngt];
        RectEval *r = new RectEval(y1, y1+h-1, x1, x1+w-1);
        rl_gt->add(r);
    }

    // Output
    plhs[0] = mxCreateDoubleMatrix(6, 1, mxREAL);
    double *scores = mxGetPr(plhs[0]);
    parseParameterString (evalParameterString, evalParameters, EV_PARAM_COUNT, (char *) "-p");
    BoxEvaluator boxEv (rl_gt, rl_det, evalParameters);
    boxEv.matchScore(&cout, rl_gt->size(),scores);
    // cout << "icdar2003 : " << scores[0] << scores[1] << scores[2] <<endl;
    // cout << "deteval   : " << scores[3] << scores[4] << scores[5] <<endl;
}
