#ifndef __LPCM_STREAM_READER_H
#define __LPCM_STREAM_READER_H

#include "avPacket.h"
#include "simplePacketizerReader.h"

class LPCMStreamReader : public SimplePacketizerReader {
 public:
  enum LPCMHeaderType { htNone, htM2TS, htWAVE, htWAVE64 };

  LPCMStreamReader() : SimplePacketizerReader(), m_useNewStyleAudioPES(false) {
    m_bitsPerSample = m_freq = m_channels = 0;
    m_lfeExists = false;
    m_headerType = htNone;
    m_testMode = false;
    m_firstFrame = true;
    m_demuxMode = false;
    // m_waveHeaderParsed = false;
    m_curChankLen = 0;
    m_channels = 0;
    m_frameRest = 0;
    m_needPCMHdr = true;
    m_openSizeWaveFormat = false;  // WAVE data size unknown and zero
    m_lastChannelRemapPos = 0;
  }
  void setNewStyleAudioPES(bool value) { m_useNewStyleAudioPES = value; }
  virtual int getTSDescriptor(uint8_t* dstBuff);
  virtual int getFreq() { return m_freq; }
  virtual int getChannels() { return m_channels; }
  // void setDemuxMode(bool value) {m_demuxMode = value;}
  void setFirstFrame(bool value) { m_firstFrame = value; }
  virtual bool beforeFileCloseEvent(File& file);
  void setHeadersType(LPCMHeaderType value);

 protected:
  virtual unsigned getHeaderLen() { return 4; }
  virtual int decodeFrame(uint8_t* buff, uint8_t* end, int& skipBytes,
                          int& skipBeforeBytes);
  virtual uint8_t* findFrame(uint8_t* buff, uint8_t* end);
  virtual double getFrameDurationNano();
  virtual const CodecInfo& getCodecInfo();
  virtual const std::string getStreamInfo();
  void writePESExtension(PESPacket* pesPacket, const AVPacket& avPacket);
  virtual void setTestMode(bool value) { m_testMode = value; }
  virtual int writeAdditionData(uint8_t* dstBuffer, uint8_t* dstEnd,
                                AVPacket& avPacket,
                                PriorityDataInfo* priorityData);
  virtual int readPacket(AVPacket& avPacket);
  virtual int flushPacket(AVPacket& avPacket);
  virtual void onSplitEvent() { m_firstFrame = true; }
  virtual void setBuffer(uint8_t* data, int dataLen, bool lastBlock = false);

 private:
  LPCMHeaderType m_headerType;
  bool m_useNewStyleAudioPES;
  bool m_testMode;
  bool m_firstFrame;
  // bool m_demuxMode;
  // bool m_waveHeaderParsed;
  uint8_t m_tmpFrameBuffer[17280 + 4];  // max PCM frame size
  uint32_t m_frameRest;
  bool m_needPCMHdr;

  int m_bitsPerSample;
  int m_freq;
  int m_channels;
  bool m_lfeExists;
  int64_t m_curChankLen;
  bool m_openSizeWaveFormat;
  uint8_t* m_lastChannelRemapPos;

  bool detectLPCMType(uint8_t* buffer, int len);
  int decodeLPCMHeader(uint8_t* buff);
  uint32_t convertLPCMToWAV(uint8_t* start, uint8_t* end);
  uint32_t convertWavToPCM(uint8_t* start, uint8_t* end);

  void storeChannelData(uint8_t* start, uint8_t* end, int chNum,
                        uint8_t* tmpData, int mch);
  void copyChannelData(uint8_t* start, uint8_t* end, int chFrom, int chTo,
                       int mch);
  void restoreChannelData(uint8_t* start, uint8_t* end, int chNum,
                          uint8_t* tmpData, int mch);
  void removeChannel(uint8_t* start, uint8_t* end, int cnNum, int mch);
  int decodeWaveHeader(uint8_t* buff, uint8_t* end);
  uint8_t* findSubstr(const char* pattern, uint8_t* buff, uint8_t* end);
};

#endif
