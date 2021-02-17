#ifndef NOWTECH_LOG_CONVERTER_CUSTOM_CHARACTER
#define NOWTECH_LOG_CONVERTER_CUSTOM_CHARACTER

#include "LogNumericSystem.h"
#include <cmath>

namespace nowtech::log {

/// Independent of STL
template<typename tMessage, bool tArchitecture64, uint8_t tAppendStackBufferSize, bool tAppendBasePrefix, bool tAlignSigned>
class ConverterCustomText final {
public:
  using tMessage_        = tMessage;
  using ConversionResult = char;
  using Iterator         = char*;
  static constexpr Iterator csNullIterator = nullptr; // Used in SenderVoid to return void begin-end pair.
  static constexpr bool     csSupportFloatingPoint = tMessage::csSupportFloatingPoint;

private:
  using IntegerConversionUnsigned = std::conditional_t<tArchitecture64, uint64_t, uint32_t>;
  using IntegerConversionSigned   = std::conditional_t<tArchitecture64, int64_t, int32_t>;

  static constexpr uint8_t csMaxDigitCountFloat      =  8u;
  static constexpr uint8_t csMaxDigitCountDouble     = 16u;
  static constexpr uint8_t csMaxDigitCountLongDouble = 34u;

  static constexpr char csNumericError            = '#';
  static constexpr char csEndOfLine               = '\n';
  static constexpr char csNumericFill             = '0';
  static constexpr char csNumericMarkBinary       = 'b';
  static constexpr char csNumericMarkHexadecimal  = 'x';
  static constexpr char csNumericMarkOther        = '!';
  static constexpr char csMinus                   = '-';
  static constexpr char csSpace                   = ' ';
  static constexpr char csFractionDot             = '.';
  static constexpr char csPlus                    = '+';
  static constexpr char csScientificE             = 'e';

  inline static constexpr char csNan[]            = "nan";
  inline static constexpr char csInf[]            = "inf";
  inline static constexpr char csTrue[]           = "true";
  inline static constexpr char csFalse[]          = "false";

  Iterator       mBegin;
  Iterator const mEnd;

public:
  ConverterCustomText(Iterator aBegin, Iterator const aEnd) noexcept
  : mBegin(aBegin)
  , mEnd(aEnd) {
  }

  ConverterCustomText(ConverterCustomText const &) = delete;
  ConverterCustomText(ConverterCustomText &&) = delete;
  ConverterCustomText& operator=(ConverterCustomText const &) = delete;
  ConverterCustomText& operator=(ConverterCustomText &&) = delete;

  Iterator end() const noexcept {
    return mBegin;
  }

  void convert(ShutdownMessageContent const, uint8_t const, uint8_t const) noexcept { // nothing to do
  }

  template <typename tDummy = void>
  auto convert(float const aValue, uint8_t const, uint8_t const aFill) noexcept -> std::enable_if_t<csSupportFloatingPoint, tDummy> {
    append(static_cast<long double>(aValue), aFill == 0u ? csMaxDigitCountFloat : aFill);
    appendSpace();
  }

  template <typename tDummy = void>
  auto convert(double const aValue, uint8_t const, uint8_t const aFill) noexcept -> std::enable_if_t<csSupportFloatingPoint, tDummy> {
    append(static_cast<long double>(aValue), aFill == 0u ? csMaxDigitCountDouble : aFill);
    appendSpace();
  }

  template <typename tDummy = void>
  auto convert(long double const aValue, uint8_t const, uint8_t const aFill) noexcept -> std::enable_if_t<csSupportFloatingPoint, tDummy> {
    append(aValue, aFill == 0u ? csMaxDigitCountLongDouble : aFill);
    appendSpace();
  }

  void convert(uint8_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionUnsigned>(aValue), static_cast<IntegerConversionUnsigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(uint16_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionUnsigned>(aValue), static_cast<IntegerConversionUnsigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(uint32_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionUnsigned>(aValue), static_cast<IntegerConversionUnsigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(uint64_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(aValue, static_cast<uint64_t>(aBase), aFill);
    appendSpace();
  }

  void convert(int8_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionSigned>(aValue), static_cast<IntegerConversionSigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(int16_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionSigned>(aValue), static_cast<IntegerConversionSigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(int32_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(static_cast<IntegerConversionSigned>(aValue), static_cast<IntegerConversionSigned>(aBase), aFill);
    appendSpace();
  }
  
  void convert(int64_t const aValue, uint8_t const aBase, uint8_t const aFill) noexcept {
    append(aValue, static_cast<int64_t>(aBase), aFill);
    appendSpace();
  }

  void convert(char const aValue, uint8_t const, uint8_t const) noexcept {
    append(aValue);
    appendSpace();
  }

  void convert(char const * const aValue, uint8_t const, uint8_t const aFill) noexcept {
    append(aValue);
    if(aFill < LogFormat::csFillValueStoreString) {   // Antipattern to use the fill for other purposes, but we go for space saving.
      appendSpace();
    }
    else { // nothing to do
    }
  }

  void convert(bool const aValue, uint8_t const, uint8_t const) noexcept {
    append(aValue ? csTrue : csFalse);
    appendSpace();
  }

  void convert(std::array<char, tMessage::csPayloadSize> const &aValue, uint8_t const, uint8_t const aFill) noexcept {
    append(aValue.data());
    if(aFill < LogFormat::csFillValueStoreString) {   // Antipattern to use the fill for other purposes, but we go for space saving.
      appendSpace();
    }
    else { // nothing to do
    }
  }

  void terminateSequence() noexcept {
    append(csEndOfLine);
  }

private:
  void appendSpace() noexcept {
    append(csSpace);
  }

  void append(char const aValue) noexcept {
    if(mBegin < mEnd) {
      *mBegin = aValue;
      ++mBegin;
    }
    else { // nothing to do
    }
  }

  void append(char const * const aValue) noexcept {
    char const * where = aValue;
    while(*where && mBegin < mEnd) { // Don't use append to be able to escape when buffer is full.
      *mBegin = *where;
      mBegin++;
      where++;
    }
  }

  inline static constexpr char csDigit2char[NumericSystem::csBaseMax] = {
    '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
  };

  template<typename tValue>
  void append(tValue const aValue, tValue const aBase, uint8_t const aFill) noexcept {
    tValue tmpValue = aValue;
    uint8_t tmpFill = aFill;
    if((aBase <= NumericSystem::csInvalid) || (aBase > NumericSystem::csBaseMax)) {
      append(csNumericError);
      return;
    }
    else { // nothing to do
    }
    if(tAppendBasePrefix) {
      if (aBase == 2u) {
        append(csNumericFill);
        append(csNumericMarkBinary);
      }
      else if(aBase == 16u) {
        append(csNumericFill);
        append(csNumericMarkHexadecimal);
      }
      else if(aBase != 10u) {
        append(csNumericFill);
        append(csNumericMarkOther);
      }
      else { // nothing to do
      }
    }
    char tmpBuffer[tAppendStackBufferSize];
    uint8_t where = 0u;
    bool negative = aValue < 0;
    do {
      tValue mod = tmpValue % aBase;
      if(mod < 0) {
        mod = -mod;
      }
      else { // nothing to do
      }
      tmpBuffer[where] = csDigit2char[mod];
      ++where;
      tmpValue /= aBase;
    } while((tmpValue != 0) && (where < tAppendStackBufferSize));
    if(where >= tAppendStackBufferSize) {
      append(csNumericError);
      return;
    }
    else { // nothing to do
    }
    if(negative) {
      append(csMinus);
    }
    else if(tAlignSigned && (aFill > 0u)) {
      append(csSpace);
    }
    else { // nothing to do
    }
    if(tmpFill > where) {
      tmpFill -= where;
      while(tmpFill > 0u) {
        append(csNumericFill);
        --tmpFill;
      }
    }
    for(--where; where > 0u; --where) {
      append(tmpBuffer[where]);
    }
    append(tmpBuffer[0]);
  }

  void append(long double const aValue, uint8_t const aDigitsNeeded) noexcept {
    if(std::isnan(aValue)) {
      append(csNan);
      return;
    } else if(std::isinf(aValue)) {
      append(csInf);
      return;
    } else if(aValue == 0.0l) {
      append(csNumericFill);
      return;
    }
    else {
      long double value = aValue;
      if(value < 0.0l) {
          value = -value;
          append(csMinus);
      }
      else if(tAlignSigned) {
        append(csSpace);
      }
      else { // nothing to do
      }
      long double exponent = floor(log10(value));
      long double normalized = value / pow(10.0l, exponent);
      int32_t firstDigit;
      for(uint8_t i = 1u; i < aDigitsNeeded; i++) {
        firstDigit = static_cast<int>(normalized);
        if(firstDigit > 9) {
          firstDigit = 9;
        }
        else { // nothing to do
        }
        append(csDigit2char[firstDigit]);
        normalized = 10.0 * (normalized - firstDigit);
        if(i == 1u) {
          append(csFractionDot);
        }
        else { // nothing to do
        }
      }
      firstDigit = static_cast<int>(round(normalized));
      if(firstDigit > 9) {
        firstDigit = 9;
      }
      else { // nothing to do
      }
      append(csDigit2char[firstDigit]);
      append(csScientificE);
      if(exponent >= 0) {
        append(csPlus);
      }
      else { // nothing to do
      }
      append(static_cast<int32_t>(exponent), static_cast<int32_t>(10), 0u);
    }
  }
};

}

#endif
