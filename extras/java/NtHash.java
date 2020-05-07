/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package btllib;

public class NtHash {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected NtHash(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NtHash obj) {
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
        btllibJNI.delete_NtHash(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public NtHash(String seq, long seq_len, long k, long hash_num) {
    this(btllibJNI.new_NtHash__SWIG_0(seq, seq_len, k, hash_num), true);
  }

  public NtHash(String seq, long k, long hash_num) {
    this(btllibJNI.new_NtHash__SWIG_1(seq, k, hash_num), true);
  }

  public boolean roll() {
    return btllibJNI.NtHash_roll(swigCPtr, this);
  }

  public SWIGTYPE_p_uint64_t hashes() {
    long cPtr = btllibJNI.NtHash_hashes(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint64_t(cPtr, false);
  }

  public long get_pos() {
    return btllibJNI.NtHash_get_pos(swigCPtr, this);
  }

  public long get_k() {
    return btllibJNI.NtHash_get_k(swigCPtr, this);
  }

  public long get_hash_num() {
    return btllibJNI.NtHash_get_hash_num(swigCPtr, this);
  }

}