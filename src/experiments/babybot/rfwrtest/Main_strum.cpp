#include "Rf.h"
#include "RFNet.h"
#include "utils.h"


#define N 500

int main()
{
	// Definizione delle variabili necessarie
	
	ifstream input_train ; 
	ifstream input_test ; 
	ofstream output; 

	
	output.open("exStrum.txt"); 


	YVector normal(2);
	YVector normal_out(1);
	normal = 1;
	normal_out = 1;

	YVector Yp(1),X(2),Y(1); // e deve avere la dimensione di una riga di Y
	double vecw;
	int i=0;
	
	RFNet prima;

	// Inizializzazione
		
	prima.Init(2,1,0,0,0,0.0000001,50,normal,normal_out);


	// Modifico alcuni parametri
	
	// Init_D

	double el[] = {25, 0.01, 0.01,25};
	prima.SetInitD (2,2,el);

	// Init_alpha

	YMatrix a(2,2);
	a = 250 * ones(2);
	prima.SetInitAlpha (a);
	
	// w_gen

	prima.SetWGen (0.2);
	
	
	prima.SetMeta (1);
	prima.SetMetaRate (250);

	
	// Training
	
	
	input_train.open ("trainSet");
	
	double mse = 0;
	
	while(1) 
	
	{
		GetInput(input_train,X,Y);
	
		if(input_train.eof ()) break;

		vecw = prima.Train (X,Y,Yp);

		if(!(prima.GetNData() % 500))
		{
			cout << "#Data: " << prima.GetNData () << "\t#Rfs: " << prima.GetNRfs () << endl;
		}
		
	}

	input_train.close ();

	input_test.open ("testSet");
	
	// Simulating

	while(1) 
	
	{
	
		GetInput(input_test,X,Y);
	
		if(input_test.eof ()) break;

		prima.Simulate (X,0.001,Yp);

		outIni(Yp,output);

	}

	input_test.close ();
	

// Stampa della struttura della rete

	//	prima.print(output); 
	//	prima.SaveNet ("strum.ini","");


	output.close();


	return 0;

}
