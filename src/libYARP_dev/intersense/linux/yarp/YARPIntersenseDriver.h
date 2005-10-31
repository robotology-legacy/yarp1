
class YARPIntersenseDriver :
  public YARPDeviceDriver<YARPNullSemaphore, YARPIntersenseDriver> 
{
public:
  YARPIntersenseDriver();

  virtual ~YARPIntersenseDriver();


  /**
   * Opens the device driver.
   * @param p is a pointer to the open arguments.
   * @return returns YARP_OK on success.
   */
  virtual int open(void *d);
  
  /**
   * Closes the device driver.
   * @return returns YARP_OK on success.
   */
  virtual int close(void);

};
