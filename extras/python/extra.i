%rename(__str__) btllib::Graph::to_string;
%rename(__bool__) btllib::SeqReader::Record::operator bool();

%feature("flatnested", "1");

%ignore btllib::DataSource::operator int();
%ignore btllib::DataSink::operator int();