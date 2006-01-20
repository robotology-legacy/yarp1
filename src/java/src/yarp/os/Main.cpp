#include <stdio.h>

#include <gcj/cni.h>
#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#include <java/lang/Throwable.h>

#include <java/lang/String.h>

#include "YarpClient.h"
#include "Time.h"

using namespace java::lang;
using namespace yarp::os;

int main(int argc, char *argv)
{

  try {
    JvCreateJavaVM(NULL);
    JvAttachCurrentThread(NULL, NULL);
    
    String *message = JvNewStringLatin1("Beginning C++ port");
    JvInitClass(&System::class$);
    System::out->println(message);

    // delete message;  Don't know how to do this
    
    JArray<String *> *array
      = (JArray<String *> *) JvNewObjectArray(1, &String::class$, NULL);
    jstring *arr = elements(array);
    arr[0] = JvNewStringUTF("server");
    
    YarpClient yc;

    Time::delay(1);
    yc.main(array);

    //delete[] array;  Don't know how to do this
    
    JvDetachCurrentThread();
  } catch (Throwable *t) {
    System::err->println(JvNewStringLatin1("Unhandled Java exception:"));
    t->printStackTrace();
  }
}
