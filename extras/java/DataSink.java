/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package btllib;

public class DataSink {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected DataSink(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
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
  }

  public DataSink(String sink, boolean append) {
    this(btllibJNI.new_DataSink(sink, append), true);
  }

  public void close() {
    btllibJNI.DataSink_close(swigCPtr, this);
  }

  public void setPipeline(_Pipeline value) {
    btllibJNI.DataSink_pipeline_set(swigCPtr, this, _Pipeline.getCPtr(value), value);
  }

  public _Pipeline getPipeline() {
    long cPtr = btllibJNI.DataSink_pipeline_get(swigCPtr, this);
    return (cPtr == 0) ? null : new _Pipeline(cPtr, false);
  }

  public void setClosed(boolean value) {
    btllibJNI.DataSink_closed_set(swigCPtr, this, value);
  }

  public boolean getClosed() {
    return btllibJNI.DataSink_closed_get(swigCPtr, this);
  }

}
