(function($) {
  
Drupal.behaviors.simsAIChatbot = {
  attach: function(context, settings) {
    
    $('.sims-ai-chatbot', context).once('sims-ai-chatbot', function() {
      var $chatbox = $(this);
      var $messages = $chatbox.find('.chat-messages');
      var $input = $chatbox.find('.chat-input');
      var $sendBtn = $chatbox.find('.chat-send');
      var conversationId = $chatbox.data('conversation-id');
      
      function sendMessage() {
        var message = $input.val();
        if (typeof message === 'string') {
          message = message.replace(/^\s+|\s+$/g, '');
        }
        
        if (message === '') {
          return;
        }
        
        $input.attr('disabled', 'disabled');
        $sendBtn.attr('disabled', 'disabled');
        
        appendMessage('user', message);
        $input.val('');
        
        var $typing = $('<div class="message ai typing"><div class="dot"></div><div class="dot"></div><div class="dot"></div></div>');
        $messages.append($typing);
        scrollToBottom();
        
        $.ajax({
          url: Drupal.settings.basePath + '?q=sims/ai-free/chatbot/message',
          type: 'POST',
          dataType: 'json',
          data: {
            conversation_id: conversationId,
            message: message
          },
          success: function(response) {
            $typing.remove();
            
            if (response.error) {
              appendMessage('error', response.error);
            } else if (response.message) {
              appendMessage('assistant', response.message);
            } else {
              appendMessage('error', 'Invalid response');
            }
            
            $input.removeAttr('disabled');
            $sendBtn.removeAttr('disabled');
            $input.focus();
          },
          error: function(xhr, status, error) {
            $typing.remove();
            appendMessage('error', 'Connection error: ' + status);
            console.log('Error:', xhr.responseText);
            $input.removeAttr('disabled');
            $sendBtn.removeAttr('disabled');
          }
        });
      }
      
      function appendMessage(role, content) {
        var $msg = $('<div class="message"></div>').addClass(role).text(content);
        $messages.append($msg);
        scrollToBottom();
      }
      
      function scrollToBottom() {
        $messages[0].scrollTop = $messages[0].scrollHeight;
      }
      
      $sendBtn.bind('click', sendMessage);
      $input.bind('keypress', function(e) {
        if (e.which === 13 && !e.shiftKey) {
          e.preventDefault();
          sendMessage();
        }
      });
      
      $input.focus();
    });
  }
};

})(jQuery);
