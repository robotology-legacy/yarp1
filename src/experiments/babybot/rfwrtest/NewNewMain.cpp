
#include "Rf.h"
#include "RFNet.h"
#include "utils.h"

#define N 20

int main()
{
	ofstream output; //****
	ifstream input ; //----
	//ofstream save ;


	output.open("o1.txt"); //***
	input.open("input.dat");//----
	//save.open("Net.ini");

	//double v[] = {0.7485,    0.6973,    0.6203}; // Se  uso TUTTA X. (N 369)
	//double v[] = {  0.7055,    0.6973,    0.6003}; // se uso i primi 100 elementi (N 100)
	 double v[] = {0.7,0.7,0.6}; // Per avere valori standard.

	YVector normal(3,v);
	YVector normal_out(1);
	normal_out = 1;
	YVector Yp(3),e(3),X(3),Y(1);
	double vecw;
	int i;
	
	RFNet prima;

	//prima.SetWPrune (0.2);
	prima.Init(3,1,0,0,0,0.0001,50,normal,normal_out,"test");

	input >> X(1);
	input >> X(2);
	input >> X(3);
	input >> Y(1);		
	
	for (i = 1; i <=N; i++) {

		
	//for (i = 1; i <=N; i++)  {// Se voglio trainare più volte sullo stesso dato!
	

		vecw = prima.Train (X,Y,Yp,e);

// Per stampare il risulato del train

//      cout << vecw << endl;
//      output << vecw << endl;
		
//		cout << Yp << endl;
//		output << Yp << endl;

//		cout << i << endl;
	

		input >> X(1);
		input >> X(2);
		input >> X(3);
		input >> Y(1);		


		prima.Simulate (X,0.001,Yp);

// Per stampare il risultato del predict

//		cout << Yp << endl;
		output << Yp << endl;
	}

	
	prima.print(output); //****
//	prima.SaveNet (save);


/*
    RFNet seconda;
	int a =seconda.LoadNet ("Net.ini","C:\\Yarp\\src\\experiments\\babybot\\RFWR\\");
	if (a == YARP_FAIL )
	{ 
		cout << "Problemi con il file "<< endl;
		return 1;
	}
	seconda.print();
*/

	output.close(); //****
	input.close();//----
//	save.close();

	return 0;

}
