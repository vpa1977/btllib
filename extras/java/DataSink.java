/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package btllib;

public class DataSink extends _Data {
  private transient long swigCPtr;

  protected DataSink(long cPtr, boolean cMemoryOwn) {
    super(btllibJNI.DataSink_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(DataSink obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        btllibJNI.delete_DataSink(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public DataSink(SWIGTYPE_p_FILE file, SWIGTYPE_p_pid_t pid) {
    this(btllibJNI.new_DataSink__SWIG_0(SWIGTYPE_p_FILE.getCPtr(file), SWIGTYPE_p_pid_t.getCPtr(pid)), true);
  }

  public DataSink(_Data d) {
    this(btllibJNI.new_DataSink__SWIG_1(_Data.getCPtr(d), d), true);
  }

  public void close() {
    btllibJNI.DataSink_close(swigCPtr, this);
  }

}