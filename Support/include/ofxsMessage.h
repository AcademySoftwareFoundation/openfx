#ifndef _ofxsMessage_H_
#define _ofxsMessage_H_

namespace OFX 
{
  namespace Message 
  {
    enum MessageReplyEnum
    {
      eMessageReplyOK,
      eMessageReplyYes,
      eMessageReplyNo,
      eMessageReplyFailed
    };

    enum MessageTypeEnum
    {
      eMessageFatal,
      eMessageError,
      eMessageMessage,
#ifdef OFX_EXTENSIONS_VEGAS
      eMessageWarning,
#endif
      eMessageLog,
      eMessageQuestion
    };
  };
};

#endif
