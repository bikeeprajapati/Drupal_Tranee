(function($) {
  
Drupal.behaviors.simsAIChatbot = {
  attach: function(context, settings) {
    
    $('.sims-ai-chatbot', context).once('sims-ai-chatbot', function() {
      var $chatbox = $(this);
      var $messages = $chatbox.find('.chat-messages');
      var $input = $chatbox.find('.chat-input');
      var $sendBtn = $chatbox.find('.chat-send');
      var conversationId = $chatbox.data('conversation-id');
      
      // Send message
      function sendMessage() {
        var message = $input.val();
        if (typeof message === 'string') {
          message = message.replace(/^\s+|\s+$/g, '');
        }
        
        if (message === '') {
          return;
        }
        
        // Disable input
        $input.attr('disabled', 'disabled');
        $sendBtn.attr('disabled', 'disabled');
        
        // Add user message to UI
        appendMessage('user', message);
        $input.val('');
        
        // Show typing indicator
        var $typing = $('<div class="message ai typing"><div class="dot"></div><div class="dot"></div><div class="dot"></div></div>');
        $messages.append($typing);
        scrollToBottom();
        
        // Send AJAX request - using jQuery's param serialization
        $.ajax({
          url: Drupal.settings.basePath + 'sims/ai-free/chatbot/message',
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
              appendMessage('error', 'Invalid response from server');
            }
            
            // Re-enable input
            $input.removeAttr('disabled');
            $sendBtn.removeAttr('disabled');
            $input.focus();
          },
          error: function(xhr, status, error) {
            $typing.remove();
            appendMessage('error', 'Connection error: ' + status + '. Please try again.');
            console.log('AJAX Error:', xhr, status, error);
            console.log('Response Text:', xhr.responseText);
            $input.removeAttr('disabled');
            $sendBtn.removeAttr('disabled');
          }
        });
      }
      
      // Append message to chat
      function appendMessage(role, content) {
        var $msg = $('<div class="message"></div>')
          .addClass(role)
          .text(content);
        $messages.append($msg);
        scrollToBottom();
      }
      
      // Scroll to bottom
      function scrollToBottom() {
        $messages[0].scrollTop = $messages[0].scrollHeight;
      }
      
      // Event handlers
      $sendBtn.bind('click', sendMessage);
      $input.bind('keypress', function(e) {
        if (e.which === 13 && !e.shiftKey) {
          e.preventDefault();
          sendMessage();
        }
      });
      
      // Initial focus
      $input.focus();
    });
  }
};

})(jQuery);
