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
      eMessageLog,
      eMessageQuestion
    };
  };
};
