#ifndef PATTY_CAKE_CAKES_PIECE_H_
#define PATTY_CAKE_CAKES_PIECE_H_

#include <cstdint>
#include <memory>
#include <string>
#include <string.h>
#include <vector>


namespace patty_cake {


struct PattyCakePiece {
  std::string sender_id;
  std::vector<uint8_t> data;
};



class PattyCakePieceSlicer {
public: // methods
  PattyCakePieceSlicer (PattyCakePiece const* piece);

  template <class T>
  T* slice () {
    auto cur_idx = idx_;
    idx_ += sizeof(T);
    return (T*)&piece_->data[cur_idx];
  }

  void slice (std::string& str);


private: // vars
  PattyCakePiece const* piece_;
  size_t idx_;
};



class PattyCakePieceMaker {
public: // getter/setter
  inline std::vector<uint8_t> const& data () const { return data_; }


public: // methods
  template <class T>
  void append (T const& input_data) {
    auto sz = sizeof(T);
    auto idx = data_.size();
    data_.resize(idx + sz);

    memcpy(&data_[idx], &input_data, sz);
  }


private: // vars
  std::vector<uint8_t> data_;
};


template<>
void PattyCakePieceMaker::append (char const* const& input_data);


template<>
void PattyCakePieceMaker::append (std::string const& input_data);



}
#endif  // ifndef PATTY_CAKE_CAKES_PIECE_H_
