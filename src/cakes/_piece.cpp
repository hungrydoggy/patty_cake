#include "./_piece.h"



namespace patty_cake {


using std::shared_ptr;



PattyCakePieceSlicer::PattyCakePieceSlicer (PattyCakePiece const* piece)
:piece_(piece),
 idx_(0)
{
}


void PattyCakePieceSlicer::slice (std::string& str) {
  auto str_size = *slice<size_t>();

  auto cur_idx = idx_;
  idx_ += str_size;

  str.resize(str_size);
  str = (char const*)&piece_->data[cur_idx];
}





void PattyCakePieceMaker::clear () {
  data_.clear();
}


void PattyCakePieceMaker::append (char const* input_data) {
  append(std::string(input_data));
}


void PattyCakePieceMaker::append (char* input_data) {
  append(std::string(input_data));
}


void PattyCakePieceMaker::append (std::string const& input_data) {

  // ready
  auto idx = data_.size();
  size_t str_size = input_data.size();

  auto ends_with_null = str_size <= 0 || input_data[str_size-1] == '\0';
  if (ends_with_null == false)
    str_size += 1;


  { // resize
    auto sz = sizeof(size_t) + str_size;
    data_.resize(idx + sz);
  }


  { // append size
    memcpy(&data_[idx], &str_size, sizeof(size_t));
  }


  // append string
  memcpy(&data_[idx + sizeof(size_t)], input_data.data(), input_data.size());
  data_[data_.size()-1] = 0;
}


}
