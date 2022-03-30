/*
 * nthash.hpp
 * Author: Hamid Mohamadi
 * Genome Sciences Centre,
 * British Columbia Cancer Agency
 */

#ifndef BTLLIB_NTHASH_HPP
#define BTLLIB_NTHASH_HPP

#include "btllib/nthash_lowlevel.hpp"
#include "btllib/status.hpp"

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace btllib {

static const char* const NTHASH_FN_NAME = "ntHash_v1";

// This lets us minimize NtHash object size. Good for performance if it's copied
// in, e.g., DBG traversal
using NTHASH_HASH_NUM_TYPE = uint8_t;
static const int NTHASH_HASH_NUM_MAX =
  std::numeric_limits<NTHASH_HASH_NUM_TYPE>::max();

using NTHASH_K_TYPE = uint16_t;
static const int NTHASH_K_MAX = std::numeric_limits<NTHASH_K_TYPE>::max();

class NtHash;
class SeedNtHash;

inline std::vector<SpacedSeed>
parse_seeds(const std::vector<std::string>& seed_strings);

inline void
parse_seeds(const std::vector<std::string>& seed_strings,
            std::vector<SpacedSeedBlocks>& blocks,
            std::vector<SpacedSeedMonomers>& monomers);

inline void
parsed_seeds_to_blocks(const std::vector<SpacedSeed>& seeds,
                       unsigned k,
                       std::vector<SpacedSeedBlocks>& blocks,
                       std::vector<SpacedSeedMonomers>& monomers);

class NtHash
{

public:
  /**
   * Constructor.
   * @param seq C string of DNA sequence to be hashed.
   * @param seq_len Length of seq.
   * @param hash_num Number of hashes to produce per k-mer.
   * @param k K-mer size.
   * @param pos Position in seq to start hashing from.
   */
  NtHash(const char* seq,
         size_t seq_len,
         unsigned hash_num,
         unsigned k,
         size_t pos = 0);

  /**
   * Constructor.
   * @param seq String of DNA sequence to be hashed.
   * @param hash_num Number of hashes to produce per k-mer.
   * @param k K-mer size.
   * @param pos Position in seq to start hashing from.
   */
  NtHash(const std::string& seq, unsigned hash_num, unsigned k, size_t pos = 0);

  NtHash(const NtHash& nthash);
  NtHash(NtHash&&) = default;

  /**
   * Calculate the hash values of current k-mer and advance to the next k-mer.
   * NtHash advances one nucleotide at a time until it finds a k-mer with valid
   * characters (ACTG) and skips over those with invalid characters (non-ACTG,
   * including N). This method must be called before hashes() is accessed, for
   * the first and every subsequent hashed kmer. get_pos() may be called at any
   * time to obtain the position of last hashed k-mer or the k-mer to be hashed
   * if roll() has never been called on this NtHash object. It is important to
   * note that the number of roll() calls is NOT necessarily equal to get_pos(),
   * if there are N or invalid characters in the hashed sequence.
   *
   * @return true on success and false otherwise.
   */
  bool roll();

  /**
   * Like the roll() function, but advance backwards.
   *
   * @return true on success and false otherwise.
   */
  bool roll_back();

  /**
   * Peeks the hash values as if roll() was called (without advancing the
   * NtHash object. The peeked hash values can be obtained through the
   * hashes() method.
   *
   * @return true on success and false otherwise.
   */
  bool peek();

  /**
   * Like peek(), but as if roll_back() was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek_back();

  /**
   * Like peek(), but as if roll(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek(char char_in);

  /**
   * Like peek(), but as if roll_back(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek_back(char char_in);

  void sub(const std::vector<unsigned>& positions,
           const std::vector<unsigned char>& new_bases);

  const uint64_t* hashes() const { return hashes_array.get(); }

  /**
   * Get the position of last hashed k-mer or the k-mer to be hashed if roll()
   * has never been called on this NtHash object.
   */
  size_t get_pos() const { return pos; }
  bool forward() const { return forward_hash <= reverse_hash; }
  unsigned get_hash_num() const { return hash_num; }
  unsigned get_k() const { return k; }

  uint64_t get_forward_hash() const { return forward_hash; }
  uint64_t get_reverse_hash() const { return reverse_hash; }

  void change_seq(const std::string& new_seq, size_t new_pos = 0)
  {
    seq = new_seq.data();
    seq_len = new_seq.length();
    pos = new_pos;
    initialized = false;
    forward_hash = 0;
    reverse_hash = 0;
  }

private:
  friend class SeedNtHash;

  /** Initialize internal state of iterator */
  bool init();

  const char* seq;
  size_t seq_len;
  const NTHASH_HASH_NUM_TYPE hash_num;
  const NTHASH_K_TYPE k;

  size_t pos;
  bool initialized;
  std::unique_ptr<uint64_t[]> hashes_array;
  uint64_t forward_hash = 0;
  uint64_t reverse_hash = 0;
};

/**
 * Similar to NtHash class, but instead of rolling on a predefined sequence,
 * BlindNtHash needs to be fed the new character on each roll. This is useful
 * when traversing an implicit de Bruijn Graph, as we need to query all  bases
 * to know the possible extensions.
 */
class BlindNtHash
{

public:
  /**
   * Constructor.
   * @param seq C string of DNA sequence to start hashing from.
   * @param seq_len Length of seq.
   * @param hash_num Number of hashes to produce per k-mer.
   * @param k K-mer size.
   * @param pos Position in seq to start hashing from.
   */
  BlindNtHash(const char* seq,
              size_t seq_len,
              unsigned hash_num,
              unsigned k,
              size_t pos = 0);

  /**
   * Constructor.
   * @param seq String of DNA sequence to start hashing from.
   * @param hash_num Number of hashes to produce per k-mer.
   * @param k K-mer size.
   * @param pos Position in seq to start hashing from.
   */
  BlindNtHash(const std::string& seq,
              unsigned hash_num,
              unsigned k,
              size_t pos = 0);

  BlindNtHash(const BlindNtHash& nthash);
  BlindNtHash(BlindNtHash&&) = default;

  /**
   * Like the NtHash::roll() function, but instead of advancing in the
   * sequence BlindNtHash object was constructed on, the provided character
   * \p char_in is used as the next base. Useful if you want to query for
   * possible paths in an implicit de Bruijn graph graph.
   *
   * @return true on success and false otherwise.
   */
  bool roll(char char_in);

  /**
   * Like the roll(char char_in) function, but advance backwards.
   *
   * @return true on success and false otherwise.
   */
  bool roll_back(char char_in);

  /**
   * Like NtHash::peek(), but as if roll(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek(char char_in);

  /**
   * Like peek(char char_in), but as if roll_back(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek_back(char char_in);

  void sub(const std::vector<unsigned>& positions,
           const std::vector<unsigned char>& new_bases);

  const uint64_t* hashes() const { return hashes_array.get(); }

  /**
   * Get the position of last hashed k-mer or the k-mer to be hashed if roll()
   * has never been called on this NtHash object.
   */
  size_t get_pos() const { return pos; }
  bool forward() const { return forward_hash <= reverse_hash; }
  unsigned get_hash_num() const { return hash_num; }
  unsigned get_k() const { return k; }

  uint64_t get_forward_hash() const { return forward_hash; }
  uint64_t get_reverse_hash() const { return reverse_hash; }

  void change_seq(const std::string& new_seq, size_t new_pos = 0)
  {
    seq_len = new_seq.length();
    std::memcpy(seq.get(), new_seq.data(), seq_len);
    pos = new_pos;
    initialized = false;
    forward_hash = 0;
    reverse_hash = 0;
  }

private:
  /** Initialize internal state of iterator */
  bool init();

  std::unique_ptr<char[]> seq;
  size_t seq_len;
  const NTHASH_HASH_NUM_TYPE hash_num;
  const NTHASH_K_TYPE k;

  size_t pos;
  bool initialized;
  std::unique_ptr<uint64_t[]> hashes_array;
  uint64_t forward_hash = 0;
  uint64_t reverse_hash = 0;
};

class SeedNtHash
{

public:
  SeedNtHash(const char* seq,
             size_t seq_len,
             const std::vector<SpacedSeed>& seeds,
             unsigned hash_num_per_seed,
             unsigned k,
             size_t pos = 0);
  SeedNtHash(const std::string& seq,
             const std::vector<SpacedSeed>& seeds,
             unsigned hash_num_per_seed,
             unsigned k,
             size_t pos = 0);
  SeedNtHash(const char* seq,
             size_t seq_len,
             const std::vector<std::string>& seeds,
             unsigned hash_num_per_seed,
             unsigned k,
             size_t pos = 0);
  SeedNtHash(const std::string& seq,
             const std::vector<std::string>& seeds,
             unsigned hash_num_per_seed,
             unsigned k,
             size_t pos = 0);

  SeedNtHash(const SeedNtHash& seed_nthash);
  SeedNtHash(SeedNtHash&&) = default;

  /**
   * Calculate the next hash value. Refer to \ref NtHash::roll() for more
   * information.
   *
   * @return true on success and false otherwise.
   */
  bool roll();

  /**
   * Like the roll() function, but advance backwards.
   *
   * @return true on success and false otherwise.
   */
  bool roll_back();

  /**
   * Peeks the hash values as if roll() was called. Refer to
   * \ref NtHash::peek() for more information.
   *
   * @return true on success and false otherwise.
   */
  bool peek();

  /**
   * Like peek(), but as if roll_back() was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek_back();

  /**
   * Like peek(), but as if roll(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek(char char_in);

  /**
   * Like peek(), but as if roll_back(char char_in) was called.
   *
   * @return true on success and false otherwise.
   */
  bool peek_back(char char_in);

  const uint64_t* hashes() const { return nthash.hashes(); }

  void change_seq(const std::string& seq, size_t pos = 0)
  {
    nthash.change_seq(seq, pos);
  }

  size_t get_pos() const { return nthash.get_pos(); }
  bool forward() const { return nthash.forward(); }
  unsigned get_hash_num() const { return nthash.get_hash_num(); }
  unsigned get_hash_num_per_seed() const { return hash_num_per_seed; }
  unsigned get_k() const { return nthash.get_k(); }

  uint64_t* get_forward_hash() const { return forward_hash.get(); }
  uint64_t* get_reverse_hash() const { return reverse_hash.get(); }

private:
  bool init();

  NtHash nthash;
  const unsigned hash_num_per_seed;

  std::vector<SpacedSeedBlocks> blocks;
  std::vector<SpacedSeedMonomers> monomers;

  std::unique_ptr<uint64_t[]> fh_no_monomers;
  std::unique_ptr<uint64_t[]> rh_no_monomers;
  std::unique_ptr<uint64_t[]> forward_hash;
  std::unique_ptr<uint64_t[]> reverse_hash;
};

inline NtHash::NtHash(const char* seq,
                      size_t seq_len,
                      unsigned hash_num,
                      unsigned k,
                      size_t pos)
  : seq(seq)
  , seq_len(seq_len)
  , hash_num(hash_num)
  , k(k)
  , pos(pos)
  , initialized(false)
  , hashes_array(new uint64_t[hash_num])
{
  check_error(k > NTHASH_K_MAX,
              "NtHash: passed k value (" + std::to_string(k) +
                ") is larger than allowed (" + std::to_string(NTHASH_K_MAX) +
                ").");
  check_error(hash_num > NTHASH_HASH_NUM_MAX,
              "NtHash: passed number of hashes (" + std::to_string(hash_num) +
                ") is larger than allowed (" +
                std::to_string(NTHASH_HASH_NUM_MAX) + ").");
  check_warning(hash_num >= k,
                "NtHash: using " + std::to_string(hash_num) +
                  " hash functions and k size of " + std::to_string(k) +
                  ". Did you permute the parameters?");
}

inline NtHash::NtHash(const std::string& seq,
                      unsigned hash_num,
                      unsigned k,
                      size_t pos)
  : NtHash(seq.c_str(), seq.size(), hash_num, k, pos)
{
}

inline NtHash::NtHash(const NtHash& nthash)
  : seq(nthash.seq)
  , seq_len(nthash.seq_len)
  , hash_num(nthash.hash_num)
  , k(nthash.k)
  , pos(nthash.pos)
  , initialized(nthash.initialized)
  , hashes_array(new uint64_t[hash_num])
  , forward_hash(nthash.forward_hash)
  , reverse_hash(nthash.reverse_hash)
{
  std::memcpy(
    hashes_array.get(), nthash.hashes_array.get(), hash_num * sizeof(uint64_t));
}

inline BlindNtHash::BlindNtHash(const char* seq,
                                size_t seq_len,
                                unsigned hash_num,
                                unsigned k,
                                size_t pos)
  : seq(new char[seq_len])
  , seq_len(seq_len)
  , hash_num(hash_num)
  , k(k)
  , pos(pos)
  , initialized(false)
  , hashes_array(new uint64_t[hash_num])
{
  check_error(k == seq_len,
              "BlindNtHash: passed sequence length (" +
                std::to_string(seq_len) + ") is not equal to k (" +
                std::to_string(k) + ").");
  check_error(k > NTHASH_K_MAX,
              "BlindNtHash: passed k value (" + std::to_string(k) +
                ") is larger than allowed (" + std::to_string(NTHASH_K_MAX) +
                ").");
  check_error(hash_num > NTHASH_HASH_NUM_MAX,
              "BlindNtHash: passed number of hashes (" +
                std::to_string(hash_num) + ") is larger than allowed (" +
                std::to_string(NTHASH_HASH_NUM_MAX) + ").");
  check_warning(hash_num >= k,
                "BlindNtHash: using " + std::to_string(hash_num) +
                  " hash functions and k size of " + std::to_string(k) +
                  ". Did you permute the parameters?");
  std::memcpy(this->seq.get(), seq, seq_len);
}

inline BlindNtHash::BlindNtHash(const std::string& seq,
                                unsigned hash_num,
                                unsigned k,
                                size_t pos)
  : BlindNtHash(seq.c_str(), seq.size(), hash_num, k, pos)
{
}

inline BlindNtHash::BlindNtHash(const BlindNtHash& nthash)
  : seq(new char[nthash.seq_len])
  , seq_len(nthash.seq_len)
  , hash_num(nthash.hash_num)
  , k(nthash.k)
  , pos(nthash.pos)
  , initialized(nthash.initialized)
  , hashes_array(new uint64_t[hash_num])
  , forward_hash(nthash.forward_hash)
  , reverse_hash(nthash.reverse_hash)
{
  std::memcpy(this->seq.get(), nthash.seq.get(), nthash.seq_len);
  std::memcpy(
    hashes_array.get(), nthash.hashes_array.get(), hash_num * sizeof(uint64_t));
}

inline SeedNtHash::SeedNtHash(const char* seq,
                              size_t seq_len,
                              const std::vector<SpacedSeed>& seeds,
                              unsigned hash_num_per_seed,
                              unsigned k,
                              size_t pos)
  : nthash(seq, seq_len, seeds.size() * hash_num_per_seed, k, pos)
  , hash_num_per_seed(hash_num_per_seed)
  , fh_no_monomers(new uint64_t[seeds.size()])
  , rh_no_monomers(new uint64_t[seeds.size()])
  , forward_hash(new uint64_t[seeds.size()])
  , reverse_hash(new uint64_t[seeds.size()])
{
  parsed_seeds_to_blocks(seeds, k, blocks, monomers);
}

inline SeedNtHash::SeedNtHash(const std::string& seq,
                              const std::vector<SpacedSeed>& seeds,
                              unsigned hash_num_per_seed,
                              unsigned k,
                              size_t pos)
  : nthash(seq, seeds.size() * hash_num_per_seed, k, pos)
  , hash_num_per_seed(hash_num_per_seed)
  , fh_no_monomers(new uint64_t[seeds.size()])
  , rh_no_monomers(new uint64_t[seeds.size()])
  , forward_hash(new uint64_t[seeds.size()])
  , reverse_hash(new uint64_t[seeds.size()])
{
  parsed_seeds_to_blocks(seeds, k, blocks, monomers);
}

inline SeedNtHash::SeedNtHash(const char* seq,
                              size_t seq_len,
                              const std::vector<std::string>& seeds,
                              unsigned hash_num_per_seed,
                              unsigned k,
                              size_t pos)
  : nthash(seq, seq_len, seeds.size() * hash_num_per_seed, k, pos)
  , hash_num_per_seed(hash_num_per_seed)
  , fh_no_monomers(new uint64_t[seeds.size()])
  , rh_no_monomers(new uint64_t[seeds.size()])
  , forward_hash(new uint64_t[seeds.size()])
  , reverse_hash(new uint64_t[seeds.size()])
{
  parse_seeds(seeds, blocks, monomers);
}

inline SeedNtHash::SeedNtHash(const std::string& seq,
                              const std::vector<std::string>& seeds,
                              unsigned hash_num_per_seed,
                              unsigned k,
                              size_t pos)
  : nthash(seq, seeds.size() * hash_num_per_seed, k, pos)
  , hash_num_per_seed(hash_num_per_seed)
  , fh_no_monomers(new uint64_t[seeds.size()])
  , rh_no_monomers(new uint64_t[seeds.size()])
  , forward_hash(new uint64_t[seeds.size()])
  , reverse_hash(new uint64_t[seeds.size()])
{
  parse_seeds(seeds, blocks, monomers);
}

inline SeedNtHash::SeedNtHash(const SeedNtHash& seed_nthash)
  : nthash(seed_nthash.nthash)
  , hash_num_per_seed(seed_nthash.hash_num_per_seed)
  , blocks(seed_nthash.blocks)
  , fh_no_monomers(new uint64_t[seed_nthash.blocks.size()])
  , rh_no_monomers(new uint64_t[seed_nthash.blocks.size()])
  , forward_hash(new uint64_t[seed_nthash.blocks.size()])
  , reverse_hash(new uint64_t[seed_nthash.blocks.size()])
{
  std::memcpy(fh_no_monomers.get(),
              seed_nthash.fh_no_monomers.get(),
              seed_nthash.blocks.size() * sizeof(uint64_t));
  std::memcpy(rh_no_monomers.get(),
              seed_nthash.rh_no_monomers.get(),
              seed_nthash.blocks.size() * sizeof(uint64_t));
  std::memcpy(forward_hash.get(),
              seed_nthash.forward_hash.get(),
              seed_nthash.blocks.size() * sizeof(uint64_t));
  std::memcpy(reverse_hash.get(),
              seed_nthash.reverse_hash.get(),
              seed_nthash.blocks.size() * sizeof(uint64_t));
}

inline std::vector<SpacedSeed>
parse_seeds(const std::vector<std::string>& seed_strings)
{
  std::vector<SpacedSeed> seed_set;
  for (const auto& seed_string : seed_strings) {
    SpacedSeed seed;
    size_t pos = 0;
    for (const auto& c : seed_string) {
      if (c != '1') {
        seed.push_back(pos);
      }
      ++pos;
    }
    seed_set.push_back(seed);
  }
  return seed_set;
}

inline void
parse_seeds(const std::vector<std::string>& seed_strings,
            std::vector<SpacedSeedBlocks>& out_blocks,
            std::vector<SpacedSeedMonomers>& out_monomers)
{
  for (const auto& seed_string : seed_strings) {
    const std::string padded_string = seed_string + '0';
    SpacedSeedBlocks care_blocks, ignore_blocks;
    std::vector<unsigned> care_monos, ignore_monos;
    unsigned i_start = 0;
    bool is_care_block = true;
    for (unsigned pos = 0; pos < padded_string.length(); pos++) {
      if (is_care_block && padded_string[pos] == '0') {
        if (pos - i_start == 1) {
          care_monos.push_back(i_start);
        } else {
          std::array<unsigned, 2> block{ { i_start, pos } };
          care_blocks.push_back(block);
        }
        i_start = pos;
        is_care_block = false;
      } else if (!is_care_block && padded_string[pos] == '1') {
        if (pos - i_start == 1) {
          ignore_monos.push_back(i_start);
        } else {
          std::array<unsigned, 2> block{ { i_start, pos } };
          ignore_blocks.push_back(block);
        }
        i_start = pos;
        is_care_block = true;
      }
    }
    unsigned num_cares = care_blocks.size() * 2 + care_monos.size();
    unsigned num_ignores = ignore_blocks.size() * 2 + ignore_monos.size() + 2;
    if (num_ignores < num_cares) {
      unsigned string_end = seed_string.length();
      std::array<unsigned, 2> block{ { 0, string_end } };
      ignore_blocks.push_back(block);
      out_blocks.push_back(ignore_blocks);
      out_monomers.push_back(ignore_monos);
    } else {
      out_blocks.push_back(care_blocks);
      out_monomers.push_back(care_monos);
    }
  }
}

inline void
parsed_seeds_to_blocks(const std::vector<SpacedSeed>& seeds,
                       unsigned k,
                       std::vector<SpacedSeedBlocks>& out_blocks,
                       std::vector<SpacedSeedMonomers>& out_monomers)
{
  std::vector<std::string> seed_strings;
  for (const SpacedSeed& seed : seeds) {
    std::string seed_string(k, '1');
    for (const auto& i : seed) {
      seed_string[i] = '0';
    }
    seed_strings.push_back(seed_string);
  }
  parse_seeds(seed_strings, out_blocks, out_monomers);
}

inline void
NtHash::sub(const std::vector<unsigned>& positions,
            const std::vector<unsigned char>& new_bases)
{
  sub_hash(forward_hash,
           reverse_hash,
           seq + pos,
           positions,
           new_bases,
           get_k(),
           get_hash_num(),
           hashes_array.get());
}

inline void
BlindNtHash::sub(const std::vector<unsigned>& positions,
                 const std::vector<unsigned char>& new_bases)
{
  sub_hash(forward_hash,
           reverse_hash,
           seq.get() + pos,
           positions,
           new_bases,
           get_k(),
           get_hash_num(),
           hashes_array.get());
}

// NOLINTNEXTLINE
#define BTLLIB_NTHASH_INIT(CLASS, NTHASH_CALL, MEMBER_PREFIX)                  \
  inline bool CLASS::init()                                                    \
  {                                                                            \
    if (MEMBER_PREFIX k > MEMBER_PREFIX seq_len) {                             \
      MEMBER_PREFIX pos = std::numeric_limits<std::size_t>::max();             \
      return false;                                                            \
    }                                                                          \
    unsigned posN = 0;                                                         \
    while (                                                                    \
      (MEMBER_PREFIX pos < MEMBER_PREFIX seq_len - MEMBER_PREFIX k + 1) &&     \
      !(NTHASH_CALL)) {                                                        \
      MEMBER_PREFIX pos += posN + 1;                                           \
    }                                                                          \
    if (MEMBER_PREFIX pos > MEMBER_PREFIX seq_len - MEMBER_PREFIX k) {         \
      MEMBER_PREFIX pos = std::numeric_limits<std::size_t>::max();             \
      return false;                                                            \
    }                                                                          \
    MEMBER_PREFIX initialized = true;                                          \
    return true;                                                               \
  }

// NOLINTNEXTLINE
#define BTLLIB_NTHASH_ROLL(CLASS, FN_DECL, NTHASH_CALL, MEMBER_PREFIX)         \
  inline bool CLASS::FN_DECL                                                   \
  {                                                                            \
    if (!MEMBER_PREFIX initialized) {                                          \
      return init();                                                           \
    }                                                                          \
    if (MEMBER_PREFIX pos >= MEMBER_PREFIX seq_len - MEMBER_PREFIX k) {        \
      return false;                                                            \
    }                                                                          \
    if (SEED_TAB[(unsigned char)(MEMBER_PREFIX seq[MEMBER_PREFIX pos +         \
                                                   MEMBER_PREFIX k])] ==       \
        SEED_N) {                                                              \
      MEMBER_PREFIX pos += MEMBER_PREFIX k;                                    \
      return init();                                                           \
    }                                                                          \
    NTHASH_CALL /* NOLINT(bugprone-macro-parentheses) */                       \
      ++ MEMBER_PREFIX pos;                                                    \
    return true;                                                               \
  }

// NOLINTNEXTLINE
#define BTLLIB_NTHASH_ROLL_BACK(CLASS, FN_DECL, NTHASH_CALL, MEMBER_PREFIX)    \
  inline bool CLASS::FN_DECL                                                   \
  {                                                                            \
    if (!MEMBER_PREFIX initialized) {                                          \
      return init();                                                           \
    }                                                                          \
    if (MEMBER_PREFIX pos <= 0) {                                              \
      return false;                                                            \
    }                                                                          \
    if (SEED_TAB[(unsigned char)(MEMBER_PREFIX seq[MEMBER_PREFIX pos - 1])] == \
        SEED_N) {                                                              \
      MEMBER_PREFIX pos -= MEMBER_PREFIX k;                                    \
      return init();                                                           \
    }                                                                          \
    NTHASH_CALL /* NOLINT(bugprone-macro-parentheses) */                       \
      -- MEMBER_PREFIX pos;                                                    \
    return true;                                                               \
  }

// NOLINTNEXTLINE
#define BTLLIB_NTHASH_PEEK(CLASS, FN_DECL, NTHASH_CALL, MEMBER_PREFIX)         \
  inline bool CLASS::FN_DECL                                                   \
  {                                                                            \
    if (!MEMBER_PREFIX initialized) {                                          \
      return init();                                                           \
    }                                                                          \
    NTHASH_CALL /* NOLINT(bugprone-macro-parentheses) */                       \
      return true;                                                             \
  }

BTLLIB_NTHASH_INIT(NtHash,
                   ntmc64(seq + pos,
                          k,
                          hash_num,
                          forward_hash,
                          reverse_hash,
                          posN,
                          hashes_array.get()), )
BTLLIB_NTHASH_ROLL(NtHash,
                   roll(),
                   ntmc64(seq[pos],
                          seq[pos + k],
                          k,
                          hash_num,
                          forward_hash,
                          reverse_hash,
                          hashes_array.get());
                   , )
BTLLIB_NTHASH_ROLL_BACK(NtHash,
                        roll_back(),
                        ntmc64l(seq[pos + k - 1],
                                seq[pos - 1],
                                k,
                                hash_num,
                                forward_hash,
                                reverse_hash,
                                hashes_array.get());
                        , )
BTLLIB_NTHASH_PEEK(
  NtHash,
  peek(),

  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64(seq[pos],
           seq[pos + k],
           k,
           hash_num,
           forward_hash_tmp,
           reverse_hash_tmp,
           hashes_array.get());
  }, )
BTLLIB_NTHASH_PEEK(
  NtHash,
  peek(char char_in),

  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64(seq[pos],
           char_in,
           k,
           hash_num,
           forward_hash_tmp,
           reverse_hash_tmp,
           hashes_array.get());
  }, )
BTLLIB_NTHASH_PEEK(
  NtHash,
  peek_back(),

  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64l(seq[pos + k - 1],
            seq[pos - 1],
            k,
            hash_num,
            forward_hash_tmp,
            reverse_hash_tmp,
            hashes_array.get());
  }, )
BTLLIB_NTHASH_PEEK(
  NtHash,
  peek_back(char char_in),

  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64l(seq[pos + k - 1],
            char_in,
            k,
            hash_num,
            forward_hash_tmp,
            reverse_hash_tmp,
            hashes_array.get());
  }, )

BTLLIB_NTHASH_INIT(BlindNtHash,
                   ntmc64(seq.get() + pos,
                          k,
                          hash_num,
                          forward_hash,
                          reverse_hash,
                          posN,
                          hashes_array.get()), )
BTLLIB_NTHASH_ROLL(
  BlindNtHash,
  roll(char char_in),
  {
    ntmc64(seq[pos % seq_len],
           char_in,
           k,
           hash_num,
           forward_hash,
           reverse_hash,
           hashes_array.get());
    seq[pos % seq_len] = char_in;
  }, )
BTLLIB_NTHASH_ROLL_BACK(
  BlindNtHash,
  roll_back(char char_in),
  {
    ntmc64l(seq[(pos + k - 1) % seq_len],
            char_in,
            k,
            hash_num,
            forward_hash,
            reverse_hash,
            hashes_array.get());
    seq[(pos + k - 1) % seq_len] = char_in;
  }, )
BTLLIB_NTHASH_PEEK(
  BlindNtHash,
  peek(char char_in),

  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64(seq[pos % seq_len],
           char_in,
           k,
           hash_num,
           forward_hash_tmp,
           reverse_hash_tmp,
           hashes_array.get());
  }, )
BTLLIB_NTHASH_PEEK(
  BlindNtHash,
  peek_back(char char_in),
  {
    uint64_t forward_hash_tmp = forward_hash;
    uint64_t reverse_hash_tmp = reverse_hash;
    ntmc64l(seq[(pos + k - 1) % seq_len],
            char_in,
            k,
            hash_num,
            forward_hash_tmp,
            reverse_hash_tmp,
            hashes_array.get());
  }, )

BTLLIB_NTHASH_INIT(SeedNtHash,
                   ntmsm64(nthash.seq + nthash.pos,
                           blocks,
                           monomers,
                           nthash.k,
                           blocks.size(),
                           hash_num_per_seed,
                           fh_no_monomers.get(),
                           rh_no_monomers.get(),
                           forward_hash.get(),
                           reverse_hash.get(),
                           posN,
                           nthash.hashes_array.get()),
                   nthash.)
BTLLIB_NTHASH_ROLL(SeedNtHash,
                   roll(),
                   ntmsm64(nthash.seq + nthash.pos,
                           blocks,
                           monomers,
                           nthash.k,
                           blocks.size(),
                           hash_num_per_seed,
                           fh_no_monomers.get(),
                           rh_no_monomers.get(),
                           forward_hash.get(),
                           reverse_hash.get(),
                           nthash.hashes_array.get());
                   , nthash.)
BTLLIB_NTHASH_ROLL_BACK(SeedNtHash,
                        roll_back(),
                        ntmsm64l(nthash.seq + nthash.pos - 1,
                                 blocks,
                                 monomers,
                                 nthash.k,
                                 blocks.size(),
                                 hash_num_per_seed,
                                 fh_no_monomers.get(),
                                 rh_no_monomers.get(),
                                 forward_hash.get(),
                                 reverse_hash.get(),
                                 nthash.hashes_array.get());
                        , nthash.)
BTLLIB_NTHASH_PEEK(
  SeedNtHash,
  peek(),

  {
    std::unique_ptr<uint64_t[]> fh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> rh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> forward_hash_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> reverse_hash_tmp(new uint64_t[blocks.size()]);
    std::memcpy(fh_no_monomers_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(rh_no_monomers_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(forward_hash_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(reverse_hash_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    ntmsm64(nthash.seq + nthash.pos,
            blocks,
            monomers,
            nthash.k,
            blocks.size(),
            hash_num_per_seed,
            fh_no_monomers_tmp.get(),
            rh_no_monomers_tmp.get(),
            forward_hash_tmp.get(),
            reverse_hash_tmp.get(),
            nthash.hashes_array.get());
  },
  nthash.)
BTLLIB_NTHASH_PEEK(
  SeedNtHash,
  peek(char char_in),

  {
    std::unique_ptr<uint64_t[]> fh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> rh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> forward_hash_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> reverse_hash_tmp(new uint64_t[blocks.size()]);
    std::memcpy(fh_no_monomers_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(rh_no_monomers_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(forward_hash_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(reverse_hash_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    ntmsm64(nthash.seq + nthash.pos,
            char_in,
            blocks,
            monomers,
            nthash.k,
            blocks.size(),
            hash_num_per_seed,
            fh_no_monomers_tmp.get(),
            rh_no_monomers_tmp.get(),
            forward_hash_tmp.get(),
            reverse_hash_tmp.get(),
            nthash.hashes_array.get());
  },
  nthash.)
BTLLIB_NTHASH_PEEK(
  SeedNtHash,
  peek_back(),

  {
    std::unique_ptr<uint64_t[]> fh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> rh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> forward_hash_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> reverse_hash_tmp(new uint64_t[blocks.size()]);
    std::memcpy(fh_no_monomers_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(rh_no_monomers_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(forward_hash_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(reverse_hash_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    ntmsm64l(nthash.seq + nthash.pos - 1,
             blocks,
             monomers,
             nthash.k,
             blocks.size(),
             hash_num_per_seed,
             fh_no_monomers_tmp.get(),
             rh_no_monomers_tmp.get(),
             forward_hash_tmp.get(),
             reverse_hash_tmp.get(),
             nthash.hashes_array.get());
  },
  nthash.)
BTLLIB_NTHASH_PEEK(
  SeedNtHash,
  peek_back(char char_in),

  {
    std::unique_ptr<uint64_t[]> fh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> rh_no_monomers_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> forward_hash_tmp(new uint64_t[blocks.size()]);
    std::unique_ptr<uint64_t[]> reverse_hash_tmp(new uint64_t[blocks.size()]);
    std::memcpy(fh_no_monomers_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(rh_no_monomers_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(forward_hash_tmp.get(),
                forward_hash.get(),
                blocks.size() * sizeof(uint64_t));
    std::memcpy(reverse_hash_tmp.get(),
                reverse_hash.get(),
                blocks.size() * sizeof(uint64_t));
    ntmsm64l(nthash.seq + nthash.pos - 1,
             char_in,
             blocks,
             monomers,
             nthash.k,
             blocks.size(),
             hash_num_per_seed,
             fh_no_monomers_tmp.get(),
             rh_no_monomers_tmp.get(),
             forward_hash_tmp.get(),
             reverse_hash_tmp.get(),
             nthash.hashes_array.get());
  },
  nthash.)

#undef BTLLIB_NTHASH_INIT
#undef BTLLIB_NTHASH_ROLL
#undef BTLLIB_NTHASH_ROLL_BACK
#undef BTLLIB_NTHASH_PEEK

} // namespace btllib

#endif