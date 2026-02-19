<?php
/**
 * @file
 * Chatbot template.
 */
?>
<div class="sims-ai-chatbot" data-conversation-id="<?php print $conversation_id; ?>">
<div class="chat-header">
<h3>AI Startup Assistant</h3>
<p>Get help refining your pitch and improving your application</p>
</div>

<div class="chat-messages">
<?php if (empty($messages)): ?>
    <div class="message assistant welcome">
    Hi! I'm your AI startup advisor. I can help you with:
    <ul>
        <li>Refining your problem statement</li>
        <li>Strengthening your market opportunity</li>
        <li>Improving your revenue model</li>
        <li>Polishing your overall pitch</li>
    </ul>
    What would you like to work on today?
    </div>
<?php else: ?>
    <?php foreach ($messages as $msg): ?>
    <div class="message <?php print $msg->role; ?>">
        <?php print nl2br(check_plain($msg->content)); ?>
    </div>
    <?php endforeach; ?>
<?php endif; ?>
</div>

<div class="chat-input-container">
<textarea class="chat-input" placeholder="Type your message..." rows="3"></textarea>
<button class="chat-send">Send</button>
</div>

<div class="chat-footer">
<small>Powered by Ollama AI • Free • Unlimited Usage</small>
</div>
</div>