%module btllib

%{
#define SWIG_FILE_WITH_INIT

#include "btllib/graph.hpp"
#include "btllib/seq_writer.hpp"
#include "btllib/seq_reader_sam_module.hpp"
#include "btllib/nthash_lowlevel.hpp"
#include "btllib/counting_bloom_filter.hpp"
#include "btllib/mi_bloom_filter.hpp"
#include "btllib/data_stream.hpp"
#include "btllib/bloom_filter.hpp"
#include "btllib/seq_reader.hpp"
#include "btllib/cstring.hpp"
#include "btllib/seq_reader_gfa2_module.hpp"
#include "btllib/indexlr.hpp"
#include "btllib/random_seq_generator.hpp"
#include "btllib/util.hpp"
#include "btllib/order_queue.hpp"
#include "btllib/seq.hpp"
#include "btllib/seq_reader_fastq_module.hpp"
#include "btllib/nthash.hpp"
#include "btllib/status.hpp"
#include "btllib/nthash_consts.hpp"
#include "btllib/process_pipeline.hpp"
#include "btllib/seq_reader_fasta_module.hpp"
#include "btllib/seq_reader_multiline_fasta_module.hpp"
#include "btllib/seq_reader_multiline_fastq_module.hpp"
%}

%include <stdint.i>
%include <typemaps.i>
%include <pyprimtypes.swg>
%include <pyopers.swg>
%include <std_common.i>
%include <cstring.i>
%include <std_string.i>
%include <exception.i>
%include <std_iostream.i>
%include <carrays.i>
%include <std_vector.i>
%include <stl.i>

%include "../extra_common.i"
%include "extra.i"

%include "btllib/graph.hpp"
%include "btllib/seq_writer.hpp"
%include "btllib/seq_reader_sam_module.hpp"
%include "btllib/nthash_lowlevel.hpp"
%include "btllib/counting_bloom_filter.hpp"
%include "btllib/mi_bloom_filter.hpp"
%include "btllib/data_stream.hpp"
%include "btllib/bloom_filter.hpp"
%include "btllib/seq_reader.hpp"
%include "btllib/cstring.hpp"
%include "btllib/seq_reader_gfa2_module.hpp"
%include "btllib/indexlr.hpp"
%include "btllib/random_seq_generator.hpp"
%include "btllib/util.hpp"
%include "btllib/order_queue.hpp"
%include "btllib/seq.hpp"
%include "btllib/seq_reader_fastq_module.hpp"
%include "btllib/nthash.hpp"
%include "btllib/status.hpp"
%include "btllib/nthash_consts.hpp"
%include "btllib/process_pipeline.hpp"
%include "btllib/seq_reader_fasta_module.hpp"
%include "btllib/seq_reader_multiline_fasta_module.hpp"
%include "btllib/seq_reader_multiline_fastq_module.hpp"
%include "../extra_templates.i"
