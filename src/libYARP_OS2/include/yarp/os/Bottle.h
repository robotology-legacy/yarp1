#ifndef _YARP2_BOTTLE_
#define _YARP2_BOTTLE_

#include <yarp/os/ConstString.h>
#include <yarp/os/Portable.h>

namespace yarp {
  namespace os {
    class Bottle;
  }
}

/**
 * A simple collection of objects that can be be
 * transported across the network in a portable way.
 * It is handy to use until you feel the need to make your own more 
 * efficient formats for transmission.
 * Objects are stored in a list, which you can add to and access.
 * To do that, you just need to make a class that is
 * a PortReader and a PortWriter.
 * The name of this comes from the idea of throwing a "message in a bottle"
 * into the network and hoping it will eventually wash ashore
 * somewhere else.  In the very early days of YARP, that is what
 * communication felt like.
 */
class yarp::os::Bottle : public Portable {
public:

  /**
   * Constructor.  The bottle is initially empty.
   */
  Bottle();

  /**
   * Creates a bottle from a string. The string which should contain a textual
   * form of the bottle, e.g. text = "10 -5.3 1.0 \"hello there\"" 
   * would give a bottle with 4 elements: an integer, two floating
   * point numbers, and a string.
   * @param text the textual form of the bottle to be interpreted
   */
  Bottle(const char *text);

  /**
   * Destructor.
   */
  virtual ~Bottle();

  /**
   * Empties the bottle of any objects it contains.
   */
  void clear();

  /**
   * Places an integer in the bottle, at the end of the list.
   * @param x the integer to add
   */
  void addInt(int x);

  /**
   * Places a floating point number in the bottle, at the end of the list.
   * @param x the number to add
   */
  void addDouble(double x);

  /**
   * Places a string in the bottle, at the end of the list.
   * @param str the string to add
   */
  void addString(const char *str);


  /**
   * Places an empty nested list in the bottle, at the end of the list.
   * The list itself is represented as a bottle.
   * @return a reference to the newly added list
   */
  Bottle& addList();

  /**
   * Reads an integer from a certain part of the list.
   * @param index the part of the list to read from
   * @return the integer - result is undefined if it is not actually an
   *         integer - see Bottle::isInt
   */
  int getInt(int index);

  /**
   * Reads a string from a certain part of the list.
   * @param index the part of the list to read from
   * @return the string - result is undefined if it is not actually an
   *         string - see Bottle::isString
   */
  ConstString getString(int index);

  /**
   * Reads a floating point number from a certain part of the list.
   * @param index the part of the list to read from
   * @return the floating point number - result is undefined if it is not
   *         actually a floating point number - see Bottle::isDouble
   */
  double getDouble(int index);

  /**
   * Reads a nested list from a certain part of the list.
   * @param index the part of the list to read from
   * @return a pointer to the nested list, or NULL if it isn't a list -
   * see Bottle::isList
   */
  Bottle *getList(int index);

  /**
   * Checks if a certain part of the list is an integer.
   * @param index the part of the list to check
   * @return true iff that part of the list is indeed an integer
   */
  bool isInt(int index);

  /**
   * Checks if a certain part of the list is a floating point number.
   * @param index the part of the list to check
   * @return true iff that part of the list is indeed a floating point number
   */
  bool isDouble(int index);

  /**
   * Checks if a certain part of the list is a string.
   * @param index the part of the list to check
   * @return true iff that part of the list is indeed a string
   */
  bool isString(int index);

  /**
   * Checks if a certain part of the list is a nested list.
   * @param index the part of the list to check
   * @return true iff that part of the list is indeed a nested list
   */
  bool isList(int index);

  /**
   * Initializes bottle from a string, which should contain a textual
   * form of the bottle, e.g. text = "10 -5.3 1.0 \"hello there\"" 
   * would give a bottle with 4 elements: an integer, two floating
   * point numbers, and a string.
   * @param text the textual form of the bottle to be interpreted
   */
  void fromString(const char *text);

  /**
   * Gives a human-readable textual representation of the bottle, suitable 
   * for passing to Bottle::fromString (see that method for examples)
   * @return a textual representation of the bottle
   */
  ConstString toString();

  /**
   * Output a representation of the bottle to a network connection.
   * @param writer the interface to the network connection for writing
   * @result true iff the representation was written successfully.
   */
  bool write(ConnectionWriter& writer);


  /**
   * Set the bottle's value based on input from a network connection.
   * @param reader the interface to the network connection for reading
   * @return true iff the bottle was read successfully.
   */
  bool read(ConnectionReader& reader);


  /*
   * Gets the number of elements in the bottle
   * @return number of elements in the bottle
   */
  int size();

private:
  void *implementation;
};

#endif

