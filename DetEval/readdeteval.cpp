/***************************************************************************
 * readeval.cpp
 *
 * Author: Christian Wolf
 *         christian.wolf@insa-lyon.fr
 *
 * Changelog:
 * 24.04.2009 cw: -bugfix: no break after many files not containing text
 * 19.04.2006 cw: -Added the comparison between two XML files
 * 27.04.2004 cw: -first version 
 *
 * 1 tab = 4 spaces
 ***************************************************************************/
 
#define VERSION		"version 2.01, 24.04.2009" 

// C
#include <math.h>
#include <unistd.h>

// C++
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <set>
#include <map>

// libxml2
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

using namespace std;

// Which type of XML file do we process?
enum FileType 
{
	FT_NO_TYPE=0,
	FT_SET,
	FT_SERIES
};

// Which of the result measures do we print?
enum PrintMeasure 
{
	PM_NO_PRINT=0,
	PM_PRINT_R,
	PM_PRINT_P,
	PM_PRINT_H,
	PM_PRINT_MODE_MSD,
	PM_PRINT_MODE_SH,
	PM_PRINT_MODE_SDW
};


// ********************************************************************
// The class for an evaluation result (possibly for a set of images)
// ********************************************************************

class EvResults 
{
	public:

		EvResults() { clear(); }
		void clear() 
		{
			crispR=crispP=crispNoGT=crispNoD=0;
			icdarR=icdarP=icdarSurfGT=icdarSurfD=0;
			surfR=surfP=surfGT=surfD=0;
			modeSSDA=modeSSDB=0;
			modeSHeight=modeSDiffWidth=0;
			modeNoPixels=modeNoNT=modeNoIgnored=0;
			id=noImages=0;
		}
		
		bool operator== (const EvResults &o) const
		{
			return (crispR==o.crispR) && (crispP==o.crispP) && 
			       (crispNoGT==o.crispNoGT) && (crispNoD==o.crispNoD) &&
			       (noImages==o.noImages);			      
		}
		
		
		bool isGreaterThan (const EvResults &o) const
		{
			return (crispR>o.crispR) || (crispP>o.crispP);
		}
		
		bool isLessThan (const EvResults &o) const
		{
			return (crispR<o.crispR) || (crispP<o.crispP);
		}

		void printOneResult (ostream &st, PrintMeasure pm) const;
        void printResultsTag(ostream &st, const char *parameterString,
        	bool doModeEstEv, bool doLatex) const;

        void add (const EvResults &e);

	public:		// DATA

		// Detection evaluation
		double crispR,crispP,crispNoGT,crispNoD;
		double icdarR,icdarP,icdarSurfGT,icdarSurfD;
		double surfR,surfP,surfGT,surfD;

		// Mode estimation evaluation
		double modeSSDA, modeSSDB;
		int modeNoPixels, modeNoIgnored;
		double modeSHeight, modeSDiffWidth;
		int modeNoNT;

		bool haveParameter;
		double parameter;

		unsigned int id;
		unsigned int noImages;
		
		string imageName;
};

// ********************************************************************
// Different functionals to sort the results
// ********************************************************************

class sortId: public binary_function<EvResults,EvResults,bool> 
{
	public:
		bool operator()(const EvResults &x, const EvResults &y) const 
		{
			return x.id < y.id;
		}
};

class sortGT: public binary_function<EvResults,EvResults,bool> 
{
	public:
		bool operator()(const EvResults &x, const EvResults &y) const 
		{
			if (x.crispNoGT!=y.crispNoGT)
				return x.crispNoGT < y.crispNoGT;
			else
				return x.id < y.id;
		}
};

class sortName: public binary_function<EvResults,EvResults,bool> 
{
	public:
		bool operator()(const EvResults &x, const EvResults &y) const 
		{
			return x.imageName < y.imageName;
		}
};

// ********************************************************************
// Add the results of one to the sum of another
// ********************************************************************

void EvResults::add (const EvResults &e) 
{
	crispR += (e.crispR * e.crispNoGT);	
    crispP += (e.crispP * e.crispNoD);
    crispNoGT += e.crispNoGT;    
    crispNoD  += e.crispNoD;
//     cerr << "AAA step: " << e.crispNoD << "-->" << crispNoD
//     	 << "   [ " << e.crispP << " " << e.crispR << " " << e.crispNoD << " " << e.crispNoGT << "]\n";
    icdarR += (e.icdarR * e.icdarSurfGT);
    icdarP += (e.icdarP * e.icdarSurfD);
    icdarSurfGT += e.icdarSurfGT;
    icdarSurfD  += e.icdarSurfD;
    surfR += (e.surfR * e.surfGT);
    surfP += (e.surfP * e.surfD);
    surfGT += e.surfGT;
    surfD  += e.surfD;
    noImages += e.noImages;
    modeSSDA += e.modeSSDA;
    modeSSDB += e.modeSSDB;
    modeNoPixels += e.modeNoPixels;
    modeNoIgnored += e.modeNoIgnored;
    modeSHeight += e.modeSHeight;
    modeSDiffWidth += e.modeSDiffWidth;
    modeNoNT += e.modeNoNT;
}

// ********************************************************************
// Print one of the results in plain ASCII text
// ********************************************************************

void EvResults::printOneResult (ostream &st, PrintMeasure pm) const 
{
	double crispH = (crispP+crispR==0 ? 0 : 2.0*crispP*crispR/(crispP+crispR));

   	if (haveParameter)
   		st << parameter << " ";

   	switch (pm) {
 		case PM_PRINT_R:
   			st << crispR << "\n";
      		break;

 		case PM_PRINT_P:
   			st << crispP << "\n";
      		break;

 		case PM_PRINT_H:
   			st << crispH << "\n";
      		break;

        case PM_PRINT_MODE_MSD:
        	st << (modeSSDA+modeSSDB)/(float)modeNoPixels << "\n";
         	break;

        case PM_PRINT_MODE_SH:
        	st << modeSHeight/(float)modeNoNT << "\n";
         	break;

        case PM_PRINT_MODE_SDW:
        	st << modeSDiffWidth/(float)modeNoNT << "\n";
         	break;
          

        default:
        	cerr << "Internal error (2).\n";
         	exit (1);
  	}
}

// ********************************************************************
// Print the results
// ********************************************************************

void EvResults::printResultsTag(ostream &st, const char *parameterString,
	bool doModeEstEv, bool doLatex) const 
{

	double divCrispR,divCrispP,divCrispH,
 		   divIcdarR, divIcdarP, divIcdarH,
		   divSurfR,divSurfP,divSurfH;
		   
// 	cerr << "AAA dividing: " << crispP << "/" << crispNoD << "=" << crispP/crispNoD << endl;

  	// score
  	divCrispR = (crispNoGT==0 ? 0 : crispR/crispNoGT);
   	divCrispP = (crispNoD==0  ? 0 : crispP/crispNoD);
    divCrispH = (divCrispP+divCrispR==0 ? 0 : 2.0*divCrispP*divCrispR/(divCrispP+divCrispR));

    // icdar 2003
    divIcdarR = (icdarSurfGT==0 ? 0 : icdarR/icdarSurfGT);
    divIcdarP = (icdarSurfD==0  ? 0 : icdarP/icdarSurfD);
    divIcdarH = (divIcdarP+divIcdarR==0 ? 0 : 2.0*divIcdarP*divIcdarR/(divIcdarP+divIcdarR));

    // score with surface
    divSurfR = (surfGT==0 ? 0 : surfR/surfGT);
   	divSurfP = (surfD==0  ? 0 : surfP/surfD);
    divSurfH = (divSurfP+divSurfR==0 ? 0 : 2.0*divSurfP*divSurfR/(divSurfP+divSurfR));

    // LaTeX format
    if (doLatex) 
    {
		st << setw(4) << setprecision(3);
		if (doModeEstEv) 
		{
			st << "MODE " << modeSSDA+modeSSDB			
			   << " & " << (modeSSDA+modeSSDB)/(float)modeNoPixels
			   << " & " << modeSHeight
			   << " & " << modeSDiffWidth
			   << " \\\\\n";
		}
		else 
		{
// 			st << "Eval-Method & Recall & Precision & Harmonic mean \\\\ \n"
// 			   << "\\hline\\\\ \n";
    		st << "ICDAR " << 100.0*divIcdarR << " & " << 100.0*divIcdarP << " & " << 100.0*divIcdarH << " \\\\\n";
    		st << "CRISP " << 100.0*divCrispR << " & " << 100.0*divCrispP << " & " << 100.0*divCrispH << " \\\\\n";
#ifdef DETAILED_EVAL_OUTPUT    		
    		st << "AREA "  << 100.0*divSurfR << " & " << 100.0*divSurfP << " & " << 100.0*divSurfH << " \\\\\n";
#endif    		
  		}
    }

    // XML format
    else 
    {

       	st	<< "  <evaluation noImages=\"" << noImages << "\"";

        if (parameterString!=NULL)
	        st << " p=\"" << parameterString << "\"";
	   	
	   	if (imageName.size()>0)
	   		st << " imageName=\"" << imageName << "\"";

        st << ">\n";

        if (doModeEstEv) 
        {
        	st	<< "    <modeEstimation "
            			<< "ssdA=\"" << modeSSDA << "\" "
                      	<< "ssdB=\"" << modeSSDB << "\" "
                        << "ssdTot=\"" << modeSSDA+modeSSDB << "\" "
                        << "msdTot=\"" << (modeSSDA+modeSSDB)/(float)modeNoPixels << "\" "
                    	<< "noPixels=\"" << modeNoPixels << "\" "
                		<< "noIgnored=\"" << modeNoIgnored << "\" "                		
                  	    << "sHeight=\"" << modeSHeight << "\" "
                        << "sDiffWidth=\"" << modeSDiffWidth << "\" "
                        << "noNT=\"" << modeNoNT << "\" "
                        << "/>\n";
        }
        
        else 
        {
        	st  << "    <icdar2003 r=\"" << divIcdarR << "\""
    			 		<< " p=\"" << divIcdarP << "\""
    			   		<< " hmean=\"" << divIcdarH << "\""
    			     	<< " noGT=\"" << icdarSurfGT   << "\""
    			   		<< " noD=\"" << icdarSurfD   << "\""
    			     	<< "/>\n"
     			<< "    <score r=\"" << divCrispR << "\""
    					<< " p=\"" << divCrispP << "\""
    					<< " hmean=\"" << divCrispH << "\""
    					<< " noGT=\"" << crispNoGT << "\""
    					<< " noD=\"" << crispNoD << "\""
    					<< "/>\n"
#ifdef DETAILED_EVAL_OUTPUT
    			<< "    <scoreSurf r=\"" << divSurfR << "\""
    			 		<< " p=\"" << divSurfP << "\""
    			   		<< " hmean=\"" << divSurfH << "\""
    			     	<< " surfGT=\"" << surfGT   << "\""
    			   		<< " surfD=\"" << surfD   << "\""
    			 		<< "/>\n"
#endif
    			;
       	}
        
    	st		<< "  </evaluation>\n";
	}
}


// ********************************************************************
// A help function
// ********************************************************************

inline char * getIdStr (xmlNodePtr node, char *id) 
{
	char *idstr = (char *) xmlGetProp(node, (const xmlChar *) id);
	if(!idstr) 
	{
		cerr << "*** Syntax error: did not find property '" << id << "'\n";
		exit (1);
    }
    return idstr;
}

// ********************************************************************
// The usage help output
// ********************************************************************

void usage (char *com) 
{
	cerr << "usage: " << com << " [ options ] input-xmlfile [ input-xmlfile2]\n"
		 << "\n"
		 << "  The XML file(s) must contain one of the following root-tags:\n"
		 << "    <evaluationSet>    Evaluations for different images: calculate the\n"
		 << "                       performance measures for the total set.\n"
		 << "    <evaluationSeries> Evaluations for different parameters. Create a\n"
		 << "                       plain text file for input to gnuplot.\n"
		 << "\n"
		 << "  OPTIONS:\n"
		 << "    [ -g ]             create a series with falling generality\n"
		 << "                       (only tresholded Prec or Rec / generality)\n"
		 << "    [ -M ]             treat mode estimation evaluation results\n"
		 << "    [ -n <count> ]     restrict number of evaluations to <count>\n"
		 << "    [ -p <par-value> ] print the given value into the \"p\" field\n"
		 << "                       when writing an output <evaluation> tag.\n"
		 << "    [ -s <prefix> ]    The prefix for the 3 files containing the\n"
		 << "                       the detailed output (recall,precision,hmean)\n"
		 << "    [ -c <cmptype> ]   compares two XML files (the second file must\n"
		 << "                       be specified!) cmptype:\n"
		 << "                       a ... all differences\n"
		 << "                       g ... results where rec or prec is greater in \n"
		 << "                             the first file\n"
		 << "                       l ... results where rec or prec is less in \n"
		 << "                             the first file\n"
		 << "    [ -L ]             print the output in LaTeX tabular format.\n"
		 << "    [ -v ]             print version information.\n"
		 ;
}

// ********************************************************************
// Help function
// ********************************************************************

inline void EatBlanks(xmlNodePtr &x)	
{
	while (xmlIsBlankNode(x))
		x=x->next;
}

// ********************************************************************
// Check the name of a node
// ********************************************************************

bool nameIsOk (xmlNodePtr node, const char *s, bool warning) 
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

// ********************************************************************
// Read the evaluations from the XML file and store them into a structure,
// which is sorted acording to a template-specified criteria
//
// The caller may chose whether the structure is a multiset or a mapAIPyramid
//
// snode .......... The first XML node
// maxCounter ..... restrict the number of images to process to this 
// doModeEstEv .... perform modeEstimation (internal for text 
//                  the wolf text detection algorithm)
// ********************************************************************

template<class T>
void readEvaluations(xmlNodePtr snode, int maxCounter, bool doModeEstEv, 
	bool doMultiSet, 
	multiset<EvResults,T> *&rv_ms,  map<string, EvResults> *&rv_map)
{
	xmlNodePtr ssnode;
	EvResults e;
	int counter;
	int allScoresFound;
	ostringstream name;
	ofstream st;

	// Create the returned structure
	if (doMultiSet)
		rv_ms = new multiset<EvResults,T>;
	else
		rv_map = new map<string, EvResults>;

    // Travers the nodes and search for the evaluation of an image
    counter=0;
    while (snode != NULL) 
    {

    	if (nameIsOk(snode, "evaluation", false)) 
    	{
     		char *pStr;

       		++counter;
         	e.clear();
          	e.id = counter;

       		ssnode = snode->children;
         	EatBlanks(ssnode);

          	// Travers the nodes and search for the score node
            allScoresFound=0;
	    	while (ssnode != NULL) 
	    	{

	     		// NODE "score"
	     		if (nameIsOk(ssnode, "score", false)) 
	     		{
	       			allScoresFound |= 1;
		       		e.crispR = atof(getIdStr(ssnode, (char *) "r"));
		         	e.crispP = atof(getIdStr(ssnode, (char *)"p"));
		           	e.crispNoGT = atof(getIdStr(ssnode, (char *)"noGT"));
		            e.crispNoD = atof(getIdStr(ssnode, (char *)"noD"));
		       	}

		        // NODE "icdar2003"
	     		if (nameIsOk(ssnode, "icdar2003", false)) 
	     		{
	       			allScoresFound |= 2;
		       		e.icdarR = atof(getIdStr(ssnode, (char *)"r"));
		         	e.icdarP = atof(getIdStr(ssnode, (char *)"p"));
		            e.icdarSurfGT = atof(getIdStr(ssnode, (char *)"noGT"));
		            e.icdarSurfD = atof(getIdStr(ssnode,(char *)"noD"));
		       	}

		        // NODE "scoreSurf"
	     		if (nameIsOk(ssnode, "scoreSurf", false)) 
	     		{
	       			allScoresFound |= 4;
		       		e.surfR = atof(getIdStr(ssnode, (char *)"r"));
		         	e.surfP = atof(getIdStr(ssnode, (char *)"p"));
		           	e.surfGT = atof(getIdStr(ssnode, (char *)"surfGT"));
		            e.surfD = atof(getIdStr(ssnode, (char *)"surfD"));
		       	}

		        // NODE "modeEstimation"
	     		if (nameIsOk(ssnode, "modeEstimation", false)) 
	     		{
	       			allScoresFound |= 8;
		       		e.modeSSDA = atof(getIdStr(ssnode, (char *)"ssdA"));
		         	e.modeSSDB = atof(getIdStr(ssnode, (char *)"ssdB"));
		          	e.modeNoPixels = atol(getIdStr(ssnode, (char *)"noPixels"));
		           	e.modeNoIgnored = atol(getIdStr(ssnode, (char *)"noIgnored"));
		            e.modeSHeight = atof(getIdStr(ssnode, (char *)"sHeight"));
		            e.modeSDiffWidth = atof(getIdStr(ssnode, (char *)"sDiffWidth"));
		            e.modeNoNT = atol(getIdStr(ssnode, (char *)"noNT"));
		       	}

		        ssnode = ssnode->next;
		       	EatBlanks(ssnode);
	     	}

          	if (doModeEstEv) 
          	{
           		if ((allScoresFound&8)!=8) 
           		{
    	       		cerr << "Found an <evaluation> node without <modeEstimation> node\n"
              			 << "which is needed for the -M option!\n"
    	         		 << "error code = " << allScoresFound << ".\n";
    	            exit (1);
    	       	}
           	}
			/*
			else {	      
    	      	if ((allScoresFound&7)!=7) {
    	       		cerr << "Found an <evaluation> node with one of the scores missing!\n"
    	         		 << "error code = " << allScoresFound << ".\n";
    	            exit (1);
              	}
	       	}
			*/

       		// let's check whether we have information about the parameters
       		pStr= (char *) xmlGetProp(snode, (const xmlChar *) "p");
       		if (pStr!=NULL) 
       		{
         		e.haveParameter=true;
         		e.parameter = atof (pStr);
         	}
           	else
           		e.haveParameter = false;

            // let's check whether we have information about the number of images
       		pStr= (char *) xmlGetProp(snode, (const xmlChar *) "noImages");
       		if (pStr!=NULL)
         		e.noImages = atol (pStr);
           	else
           		e.noImages = 1;
           		
           	// and on the image name
       		pStr= (char *) xmlGetProp(snode, (const xmlChar *) "imageName");
       		if (pStr!=NULL)
         		e.imageName = pStr;
           	else
           		e.imageName = "";

            // Add the evaluation node to the result set
            if (doMultiSet)
         		rv_ms->insert (e);
         	else
         	{
         		if (pStr==NULL)
         		{
         			cerr << "Found an <evaluation> node without imageName property!\n"
              			 << "This property is needed for the comparison of different\n"
    	         		 << "evaluation results!\n";
    	            exit (1);
         		}
         		else
         			(*rv_map)[e.imageName] = e;
         	}
     	}

      	// The user restricted the number of images to process
      	if (maxCounter>0 && counter>=maxCounter) 
      	{
       		cerr << "Stopping accumulation after " << maxCounter << "results\n"
         		 << "due to user request.\n";
            cerr.flush();
            break;
       	}

        snode = snode->next;
       	EatBlanks(snode);
	}
}


// ********************************************************************
// Travers the evaluations once
// ********************************************************************

template <class T>
EvResults processSeries(multiset<EvResults,T> &evs, PrintMeasure pm, char *filename) 
{
	EvResults rv;
	ostringstream name;
	ofstream st;

	// Open the output files
	if (filename!=NULL && pm!=PM_NO_PRINT) 
	{

     	// Create the filename
      	name << filename;
       	switch (pm) {
        	case PM_PRINT_R: 		name << "Recall"; 					break;
         	case PM_PRINT_P: 		name << "Precision"; 				break;
         	case PM_PRINT_H: 		name << "Harmonic mean"; 			break;
          	case PM_PRINT_MODE_MSD: name << "Mode: mean squared error"; break;
            case PM_PRINT_MODE_SH: 	name << "Mode: sum height"; 		break;
            case PM_PRINT_MODE_SDW: name << "Mode: sum diff. width"; 	break;
          	default: case PM_NO_PRINT: break;
        }

 		st.open (name.str().c_str(),  ios::out);
     	if (!st.good()) 
     	{
     		cerr << "Cannot open file '" << filename << "' for writing!\n";
     		exit (1);
     	}
   	}

    // Travers the nodes and search for the evaluation of an image
    rv.clear();
    for (typename multiset<EvResults,T>::iterator e=evs.begin(); e!=evs.end(); ++e) 
    {

		rv.add (*e);

      	if (filename!=NULL && pm!=PM_NO_PRINT)
       		e->printOneResult (st, pm);
	}

	if (filename!=NULL && pm!=PM_NO_PRINT)
		st.close();

	return rv;
}

// ********************************************************************
// Travers the evaluations once
// ********************************************************************

EvResults processSet (ostream &st, bool doPrintGeneralitySeries, bool doModeEstEv,
	multiset<EvResults,sortGT> &evs) 
	{

	bool weReachedZeros=false;
	EvResults sum;
	int no_images=0,
		no_images_void=0;

	if (doPrintGeneralitySeries) 
	{
		st << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		   << "<evaluationSeries>\n";
	}

    // Travers the nodes and search for the evaluation of an image
    sum.clear();
    multiset<EvResults,sortGT>::iterator e=evs.end();
    while (e!=evs.begin()) 
    {

    	--e;

     	// We reached the first image without text in the groundtruth
      	// -> Write the performance before adding this image
		if (!doModeEstEv && !weReachedZeros && e->crispNoGT==0) 
		{
			weReachedZeros=true;
			if (doPrintGeneralitySeries) 
			{
				ostringstream gStr;
				gStr << (double) no_images / sum.crispNoGT;
				sum.printResultsTag (st, gStr.str().c_str(), doModeEstEv, false);
			}
      	}

     	// add the current image
      	sum.add (*e);
      	++no_images;
       	if (e->crispNoGT==0)
        	++no_images_void;

     	if (doPrintGeneralitySeries && weReachedZeros) 
     	{
      		ostringstream gStr;
      		gStr << (double) no_images / sum.crispNoGT;
			sum.printResultsTag (st, gStr.str().c_str(), doModeEstEv, false);
		}

		// We have reached the maximum level of
        // non-zero images
        if (doPrintGeneralitySeries && !doModeEstEv && 
            weReachedZeros && (no_images_void >= (no_images-no_images_void)))
        	break;
		
	}

	if (doPrintGeneralitySeries)
		st << "</evaluationSeries>" << endl;

	if (no_images<1)
		cerr << "No images processed!\n";
	else
    	cerr << "Included " << no_images-no_images_void << " images with non-zero groundtruth\n"
    		 << "Included " << no_images_void << " images with zero groundtruth\n"
    		 << "Skipped " << evs.size()-no_images << " images with zero groundtruth.\n"
			 << "Total-Number-Of-Processed-Images: " << no_images << endl 			 
    		 << (100.0*(double)(no_images-no_images_void))/(double)no_images
    		 << "% of the images contain objects.\n"
    		 << "Generality: " << sum.crispNoGT / (double) no_images << endl
    		 << "Inverse-Generality: " << (double) no_images / sum.crispNoGT << endl;
    		 ;

	return sum;
}	

// ********************************************************************
// Compare two evaluation sets
// ********************************************************************

void compareSets (map<string, EvResults> &set1, map<string, EvResults> &set2, char compareType)
{
	map<string, EvResults>::iterator iter1, iter2;
		
	// Travers the first set
	for (iter1=set1.begin(); iter1!=set1.end(); ++iter1) 
	{		
		// Search the entry in the second set
		iter2 = set2.find(iter1->first);
		
		// The entry exists in the first set but not in the second
		if (iter2 == set2.end()) 
		{			
			cout << "====== <<< (insert):\n";
			iter1->second.printResultsTag(cout, NULL, false, false);
		}
		
		// The entry exists in both sets
		else 
		{	
			if (!(iter1->second == iter2->second))
			{
				if (compareType=='a' || 
				    (compareType=='g' && iter1->second.isGreaterThan(iter2->second)) ||
				    (compareType=='l' && iter1->second.isLessThan(iter2->second)))
				{
					cout << "====== <<< (diff):\n";
					iter1->second.printResultsTag(cout, NULL, false, false);
					cout << "====== >>> (diff):\n";
					iter2->second.printResultsTag(cout, NULL, false, false);
				}
			}	
			
			// Remove the entry in the second set
			if (set2.erase (iter2->first)!=1) 
			{
				cerr << "Internal error: could not erase evaluation entry!" << endl;	 
				exit (1);
			}
		}		
	}	
	
	// The entries which are in the second set now have not been in the 
	// first set
	for (iter2=set2.begin(); iter2!=set2.end(); ++iter2) 
	{		
		cout << "====== >>> (delete):\n";
		iter2->second.printResultsTag(cout, NULL, false, false);
	}
}

// ********************************************************************
// read and check the XML document
// ********************************************************************

static FileType readAndCheckDocument (char *inputfilename, xmlDocPtr &doc, xmlNodePtr &root)
{	
	FileType fileType;	
	
	// Load and parse the document
	doc = xmlParseFile(inputfilename);
	if (doc == NULL) 
	{
		cerr << "Couldn't parse document " << inputfilename << endl;
		exit (1);
	}

	// Check the document type
	root = xmlDocGetRootElement(doc);
	fileType=FT_NO_TYPE;
	if (nameIsOk(root, "evaluationSet", false))
		fileType = FT_SET;
	if (nameIsOk(root, "evaluationSeries", false))
		fileType = FT_SERIES;
		
	if (fileType==FT_NO_TYPE) 
	{
		cerr << "Invalid root node in document '" << inputfilename << "'!\n";
		exit (1);
	}
	
	return fileType;
}

// ********************************************************************
// The main function
// ********************************************************************

int main (int argc, char **argv) 
{
	xmlDocPtr doc,doc2;
	xmlNodePtr snode, snode2, root, root2;	
	char *optParameterString=NULL,
		 *optTextOutputPrefix=(char *)"";
	bool optDoGeneralitySeries=false,
		 optDoLaTeXOutput=false,
		 optModeEvaluation=false,
		 optDoCompare=false;
	char *inputfilename, *inputfilename2;
	char compareType='a';
	int c, maxCounter;
	EvResults total;
	FileType fileType, fileType2;
	multiset<EvResults,sortId> *evsi;
	multiset<EvResults,sortGT> *evsg, *dummys;
	map<string, EvResults> *evm1, *evm2, *dummym;

    //	***************************************************
	//	Arguments
	//	***************************************************

	maxCounter=-1;
    while ((c =	getopt (argc, argv,	"Mn:p:s:gLc:v")) != EOF) 
    {

		switch (c) 
		{

			case 'v':
					cerr << *argv << " " << VERSION << endl;				
					return 0;

    		case 'n':				
				maxCounter=atol(optarg);
				break;

			case 'p':
				optParameterString=optarg;
				break;

			case 's':
				optTextOutputPrefix=optarg;
				break;

			case 'g':
				optDoGeneralitySeries=true;
				break;

			case 'L':
				optDoLaTeXOutput=true;
				break;

			case 'M':
				optModeEvaluation=true;
				break;
				
			case 'c':
				optDoCompare=true;
				compareType=*optarg;
				break;

			default:
				usage (*argv);
    			cerr << "unsupported option: " << c << endl;
				exit (1);
		}
	}

	// Check the arguments
	if ((!optDoCompare && (argc-optind != 1) ) || (optDoCompare && (argc-optind != 2)))
	{
	 	usage (*argv);
	 	exit(1);
	}
	inputfilename=argv[optind];
	inputfilename2=argv[optind+1];
	
	fileType = readAndCheckDocument(inputfilename, doc, root);
	if (optDoCompare)
		fileType2 = readAndCheckDocument(inputfilename2, doc2, root2);
	
	if (fileType==FT_SERIES && optDoGeneralitySeries) 
	{
		usage (*argv);
		cerr << "\nCannot create a generality series out of a <evaluationSeries> XML file!\n";
		exit (1);
	}
				
	snode=root->children;
    EatBlanks(snode);
    
    // ---------------------------------------------------------------------------
    // We compare two XML files
    // ---------------------------------------------------------------------------
    
    if (optDoCompare)
    {
    	snode2=root2->children;
    	EatBlanks(snode2);
    
    	readEvaluations(snode,  0, false, false, dummys, evm1);
    	readEvaluations(snode2, 0, false, false, dummys, evm2);
    	
    	compareSets(*evm1, *evm2, compareType);
    	delete evm1;
    	delete evm2;
	}
	
	// ---------------------------------------------------------------------------
    // We process only a single XML file
    // ---------------------------------------------------------------------------
    
    else
	{
		switch (fileType) 
		{
	
			case FT_SET:
	
				// We need to create a series with falling generality
				if (optDoGeneralitySeries) 
				{
					readEvaluations<sortGT>(snode, maxCounter, optModeEvaluation, true, evsg, dummym);
					processSet (cout, true, optModeEvaluation, *evsg);
					delete evsg;
				}
	
				// We read them all in
				else 
				{
					readEvaluations<sortGT>(snode, maxCounter, optModeEvaluation, true, evsg, dummym);
					total = processSet (cout, false, optModeEvaluation, *evsg);
					total.printResultsTag(cout, optParameterString,
						optModeEvaluation, optDoLaTeXOutput);
					delete evsg;
				}
				break;
	
			case FT_SERIES:
				readEvaluations<sortId>(snode, maxCounter, optModeEvaluation, true, evsi, dummym);
				if (optModeEvaluation) 
				{
					processSeries (*evsi, PM_PRINT_MODE_MSD, optTextOutputPrefix);
					processSeries (*evsi, PM_PRINT_MODE_SH, optTextOutputPrefix);
					processSeries (*evsi, PM_PRINT_MODE_SDW, optTextOutputPrefix);
				}
				else 
				{
					processSeries (*evsi, PM_PRINT_R, optTextOutputPrefix);
					processSeries (*evsi, PM_PRINT_P, optTextOutputPrefix);
					processSeries (*evsi, PM_PRINT_H, optTextOutputPrefix);
				}
				delete evsi;
				break;
	
			default:
				cerr << "Internal error (1).\n";
				exit (1);
		}
	}
	
	// clean up
	xmlFreeDoc(doc);

 	return 0;
}

