(function($) {

/**
 * Startup screener functionality.
 */
Drupal.behaviors.simsAIScreener = {
attach: function(context, settings) {

$('.sims-ai-screen-button', context).once('sims-ai-screener', function() {
    var $button = $(this);
    var nodeId = $button.data('node-id');
    
    $button.on('click', function(e) {
    e.preventDefault();
    
    if ($button.hasClass('loading')) {
        return;
    }
    
    // Show loading state
    $button.addClass('loading');
    $button.text('Screening with AI...');
    $button.prop('disabled', true);
    
    // Show loading indicator
    var $container = $('.screener-result-container');
    $container.html('<div class="loading-message"><div class="spinner"></div><p>AI is analyzing this startup application. This may take 5-10 seconds...</p></div>');
    
    // Make AJAX request
    $.ajax({
        url: Drupal.settings.basePath + 'sims/ai-free/screen/' + nodeId,
        method: 'GET',
        dataType: 'json',
        timeout: 30000, // 30 second timeout
        success: function(response) {
        if (response.success) {
            displayScreeningResult(response.data);
        } else {
            showError(response.message || 'Screening failed. Please try again.');
        }
        },
        error: function(xhr, status, error) {
        if (status === 'timeout') {
            showError('Request timed out. The AI might be busy. Please try again.');
        } else {
            showError('Connection error. Please check if Ollama is running.');
        }
        },
        complete: function() {
        $button.removeClass('loading');
        $button.text('Screen with AI');
        $button.prop('disabled', false);
        }
    });
    });
    
    function displayScreeningResult(data) {
    var html = '<div class="screener-result">';
    html += '<h3>AI Screening Result</h3>';
    html += '<div class="screening-summary">';
    html += '<pre>' + escapeHtml(data.summary) + '</pre>';
    html += '</div>';
    html += '<div class="screening-meta">';
    html += '<p><strong>Screened:</strong> ' + formatTimestamp(data.screened_at) + '</p>';
    html += '<p><strong>Provider:</strong> ' + data.provider + '</p>';
    html += '</div>';
    html += '<div class="screening-actions">';
    html += '<button class="button refresh-screening">Refresh Analysis</button>';
    html += '</div>';
    html += '</div>';
    
    $('.screener-result-container').html(html);
    
    // Bind refresh button
    $('.refresh-screening').on('click', function() {
        $('.sims-ai-screen-button').trigger('click');
    });
    }
    
    function showError(message) {
    var html = '<div class="screener-error">';
    html += '<div class="error-icon">⚠️</div>';
    html += '<h3>Screening Failed</h3>';
    html += '<p>' + escapeHtml(message) + '</p>';
    html += '<button class="button retry-screening">Try Again</button>';
    html += '</div>';
    
    $('.screener-result-container').html(html);
    
    $('.retry-screening').on('click', function() {
        $('.sims-ai-screen-button').trigger('click');
    });
    }
    
    function escapeHtml(text) {
    var div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
    }
    
    function formatTimestamp(timestamp) {
    var date = new Date(timestamp * 1000);
    return date.toLocaleString();
    }
});

// Auto-load existing screening if present
$('.screener-result-cached', context).once('screener-cached', function() {
    // Already displayed from PHP
});
}
};

})(jQuery);