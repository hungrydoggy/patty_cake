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


void PattyCakePieceSlicer::slice (std::vector<uint8_t>& buffer) {
  auto buffer_size = *slice<size_t>();

  auto cur_idx = idx_;
  idx_ += buffer_size;

  buffer.resize(buffer_size);
  std::memcpy(buffer.data(), &piece_->data[cur_idx], buffer_size);
}


void PattyCakePieceSlicer::clear () {
  idx_ = 0;
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
    std::memcpy(&data_[idx], &str_size, sizeof(size_t));
  }


  // append string
  std::memcpy(&data_[idx + sizeof(size_t)], input_data.data(), input_data.size());
  data_[data_.size()-1] = 0;
}


void PattyCakePieceMaker::append (uint8_t* buffer, size_t buffer_size) {
  // ready
  auto idx = data_.size();


  { // resize
    auto sz = sizeof(size_t) + buffer_size;
    data_.resize(idx + sz);
  }


  // append buffer_size
  std::memcpy(&data_[idx], &buffer_size, sizeof(size_t));


  // append buffer
  std::memcpy(&data_[idx + sizeof(size_t)], buffer, buffer_size);
}


}
