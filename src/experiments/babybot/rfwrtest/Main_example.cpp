/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: Main_example.cpp,v 1.2 2005-06-17 14:27:45 gmetta Exp $
///
///

#include <yarp/Rf.h>
#include <yarp/RFNet.h>
#include <yarp/utils.h>

#define N 500

int main()
{
	ofstream output; 
	ifstream input_train ; 
	ifstream input_test ; 
	
	//ofstream save ;


	output.open("ex.txt"); 
	input_train.open("trainSet");
	
	
	//save.open("Net.ini");

	YVector normal(2);
	YVector normal_out(1);
	normal = 1;
	normal_out = 1;

	// e deve avere la dimensione di una riga di Y

	YVector Yp(1),e(1),X(2),Y(1);
	double vecw;
	int i;
	
	RFNet prima;

	
	//  rfwr('Init',ID,2,1,0,0,0,1e-7,50,[1;1],[1],'rfwr_test');
	prima.Init(2,1,0,0,0,0.0000001,50,normal,normal_out,"rfwr_test");


	
	//rfwr('Change',ID,'init_D',[25 0.01; 0.01 25]); 
	double el[] = {25, 0.01, 0.01,25};
	prima.SetInitD (2,2,el);

	//rfwr('Change',ID,'init_alpha',ones(2)*250);     
	YMatrix a(2,2);
	a = 250 * ones(2);
	prima.SetInitAlpha (a);

	
	//rfwr('Change',ID,'w_gen',0.2);                  
	prima.SetWGen (0.2);
	
	//rfwr('Change',ID,'meta',1);                     
//	prima.SetMeta (1);

	//rfwr('Change',ID,'meta_rate',250);
//	prima.SetMetaRate (250);


	for (i = 1; i <=N*20; i++) {

	input_train >> X(1);
	input_train >> X(2);
	
	input_train >> Y(1);		
	
	
	vecw = prima.Train (X,Y,Yp,e);

	// Per stampare il risulato del train

//      cout << vecw << endl;
//      output << vecw << endl;
		
//		cout << Yp << endl;
//		output << Yp << endl;

//		cout << i << endl;
		
	
	}

	input_train.close();

	input_test.open("testSet");


	for (i = 1; i <=1681; i++) {

	input_test >> X(1);
	input_test >> X(2);

	input_test >> Y(1);

	//[yp,w]=rfwr('Predict',ID,Xt(i,:)',0.001);
	prima.Simulate (X,0.001,Yp);

// Per stampare il risultato del predict

//		cout << Yp << endl;
//		output << Yp << endl;

	outIni(Yp,output);
	}


	
//	prima.print(output); //****
//	prima.SaveNet (save);


	output.close();

	input_test.close();
//	save.close();

	return 0;

}
