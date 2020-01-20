
//  R. Lica -  Reading parameters from config file



// R. Lica, 02.03.2016 - Reading Source type from config file


#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <istream>
#include <ostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>


int GLOB_source_nb        ; //number of sources
int GLOB_source_type[10]  ; //type of each source
int GLOB_source_energy[10]; //energy of each source

int  GLOB_nb_ClKU           ; //1
int  GLOB_nb_ClBuch         ; //2	 
int  GLOB_nb_LaBr3          ; //3	
int  GLOB_nb_RightPlasticKU ; //4	
int  GLOB_nb_LeftPlasticKU  ; //5
int  GLOB_nb_FrontPlasticKU ; //6
int  GLOB_nb_FPlastic       ; //7	 
int  GLOB_nb_Tape	    	; //8
int  GLOB_nb_ImpChamber	    ; //9
int  GLOB_nb_Polyhedron	    ; //10
int  GLOB_nb_Cubic_Chamber  ; //11	 
int  GLOB_nb_IS530_Chamber  ; //12	 
int  GLOB_nb_IS530_Plastic  ; //13	 
int  GLOB_nb_VetoPlastic    ; //14	 
int  GLOB_nb_VandleBar      ; //15

int	gLines;
int	gType[1000];
double	gDistance[1000];
double	gTheta[1000];
double	gPhi[1000];
double	gSpin[1000];




using namespace std;

typedef struct {
	int	type;
	double	distance;
	double	theta;
	double	phi;
	double	spin;
}config_line;

void read_config(string cFileName) {
	
	cout << "Start reading the config.dat file" << endl;
	
	int clmax=6;  // column number in the config.dat file
	char *fline;
	char comment_string[100][100];
	double config_coding[1000][clmax]; //config_coding[lines][columns]
    int line=0, column=0;

	std::ifstream f;
	std::cout << cFileName.c_str() << std::endl;
	f.open(cFileName.c_str(), std::ifstream::in);
	if (!f.good()) {
		std::cerr << "Error ! File config.dat does not exist: "<< std::endl;
		exit(1);
	}

	else{
		std::cout << "The file config.dat is opened." << std::endl;
	}
	

	//Reading the config.dat file
    while (!f.eof()) {
		std::string fline;
		fline.clear();
		if(getline(f, fline).rdbuf()->in_avail() == 0){
				break;
		}
		
		std::string str;
		str.clear();
		std::stringstream fline_str(fline);
		
		fline_str >> str;
                
        if(('#'  == (str.substr(0)).at(0)) ){ // Skip the comment sign
            continue;
		}
		
		if(('p'  == (str.substr(0)).at(0)) ){ // Reading the particle source type
            fline_str >> GLOB_source_type[GLOB_source_nb];
            fline_str >> GLOB_source_energy[GLOB_source_nb];
            cout << endl <<"Source type: " << GLOB_source_type[GLOB_source_nb] << "\tEnergy: " << GLOB_source_energy[GLOB_source_nb] << " keV" << endl;
            GLOB_source_nb++;
            continue;            
		}
		
		fline_str.seekg(0,ios::beg);
		
		while(column<(clmax-1)){			 // Reading definitions
			fline_str >> config_coding[line][column];
			column++;
		}
		
		fline_str >> comment_string[line];
		column=0;
		line++;
    }
    
    gLines = line;
    cout << endl << "Finished reading "<<line<<" lines from the config.dat file" << endl;
	
	for(int i=0;i<line;i++){
		for(int j=0;j<clmax;j++){
			if(0==j) gType[i]		= config_coding[i][j];
			if(1==j) gDistance[i]	= config_coding[i][j];
			if(2==j) gTheta[i]		= config_coding[i][j];
			if(3==j) gPhi[i]		= config_coding[i][j];
			if(4==j) gSpin[i]		= config_coding[i][j];
		}
		
		for(int j=0;j<clmax;j++){
			if(0==j) cout << gType[i]		<< "\t";
			if(1==j) cout << gDistance[i]	<< "\t";
			if(2==j) cout << gTheta[i]		<< "\t";
			if(3==j) cout << gPhi[i]		<< "\t";
			if(4==j) cout << gSpin[i]		<< endl;
		}
		
	}
	
	for (int i = 0; i<GLOB_source_nb; i++){
	if (GLOB_source_type[i] < 1 || GLOB_source_type[i] > 4) {
				            cout << endl <<"ERROR - Invalid source type:" << GLOB_source_type[i] << "Source nb: " << i << endl;
				            exit(0);
						}	
	if (GLOB_source_energy[i] < 0 || GLOB_source_energy[i] > 50000) {
				            cout << endl <<"ERROR - Invalid source energy (0 - 50 MeV):" << GLOB_source_energy[i] << "Source nb: " << i << endl;
				            exit(0);
						}	
						
					}
	
	
	for(int i=0;i<line;i++){
		
		if (gType[i] == 1)	GLOB_nb_ClKU++;
		if (gType[i] == 2)	GLOB_nb_ClBuch++;
		if (gType[i] == 3)	GLOB_nb_LaBr3++;
		if (gType[i] == 4)	GLOB_nb_RightPlasticKU++;
		if (gType[i] == 5)	GLOB_nb_LeftPlasticKU++;
		if (gType[i] == 6)	GLOB_nb_FrontPlasticKU++;
		if (gType[i] == 7)	GLOB_nb_FPlastic++;
		if (gType[i] == 8)	GLOB_nb_Tape++;
		if (gType[i] == 9)	GLOB_nb_ImpChamber++;
		if (gType[i] == 10)	GLOB_nb_Polyhedron++;
		if (gType[i] == 11)	GLOB_nb_Cubic_Chamber++;
		if (gType[i] == 12)	GLOB_nb_IS530_Chamber++;
		if (gType[i] == 13)	GLOB_nb_IS530_Plastic++;
		if (gType[i] == 14)	GLOB_nb_VetoPlastic++;
		
	}
	
	cout << "Assigned config values from the config.dat file" << endl;
	
	// Print Assigned values
	
	std::cout<<"\nGLOB_nb_ClKU = "				<<GLOB_nb_ClKU				;
	std::cout<<"\nGLOB_nb_ClBuch = "			<<GLOB_nb_ClBuch			;	
	std::cout<<"\nGLOB_nb_LaBr3 = "				<<GLOB_nb_LaBr3				;
	std::cout<<"\nGLOB_nb_RightPlasticKU = "	<<GLOB_nb_RightPlasticKU	;
	std::cout<<"\nGLOB_nb_LeftPlasticKU = "		<<GLOB_nb_LeftPlasticKU		;
	std::cout<<"\nGLOB_nb_FronttPlasticKU = "	<<GLOB_nb_FrontPlasticKU	;
	std::cout<<"\nGLOB_nb_FPlastic = "			<<GLOB_nb_FPlastic			;
	std::cout<<"\nGLOB_nb_Tape = "				<<GLOB_nb_Tape				;
	std::cout<<"\nGLOB_nb_ImpChamber = "		<<GLOB_nb_ImpChamber		;
	std::cout<<"\nGLOB_nb_Polyhedron = "		<<GLOB_nb_Polyhedron		;
	std::cout<<"\nGLOB_nb_Cubic_Chamber = "		<<GLOB_nb_Cubic_Chamber		;
	std::cout<<"\nGLOB_nb_IS530_Chamber = "		<<GLOB_nb_IS530_Chamber		;
    std::cout<<"\nGLOB_nb_IS530_Plastic = "		<<GLOB_nb_IS530_Plastic		;
    std::cout<<"\nGLOB_nb_VetoPlastic = "		<<GLOB_nb_VetoPlastic		;


    
    


}
  
  
  
  
	
	
  

