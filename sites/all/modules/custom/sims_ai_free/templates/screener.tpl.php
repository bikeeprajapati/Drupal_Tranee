<?php
/**
 * @file
 * Template for screening interface.
 */
?>
<div class="sims-ai-screener">
<div class="screener-header">
<h2>AI-Powered Startup Screening</h2>
<p>Get instant AI analysis of this startup application</p>
</div>

<div class="startup-info">
<h3><?php print check_plain($node->title); ?></h3>
<div class="meta">
    <?php if (isset($node->field_industry[LANGUAGE_NONE][0]['value'])): ?>
    <span class="industry"><strong>Industry:</strong> <?php print check_plain($node->field_industry[LANGUAGE_NONE][0]['value']); ?></span>
    <?php endif; ?>
    <?php if (isset($node->field_stage[LANGUAGE_NONE][0]['value'])): ?>
    <span class="stage"><strong>Stage:</strong> <?php print check_plain($node->field_stage[LANGUAGE_NONE][0]['value']); ?></span>
    <?php endif; ?>
    <span class="submitted"><strong>Submitted:</strong> <?php print format_date($node->created, 'short'); ?></span>
</div>
</div>

<div class="screener-controls">
<button class="button button-primary sims-ai-screen-button" data-node-id="<?php print $node->nid; ?>">
    Screen with AI
</button>
<p class="help-text">Click to analyze this application with AI (~5-10 seconds)</p>
</div>

<div class="screener-result-container">
<?php if (isset($existing_screening)): ?>
    <div class="screener-result screener-result-cached">
    <h3>AI Screening Result</h3>
    <div class="screening-summary">
        <pre><?php print check_plain($existing_screening['summary']); ?></pre>
    </div>
    <div class="screening-meta">
        <p><strong>Screened:</strong> <?php print format_date($existing_screening['screened_at'], 'medium'); ?></p>
        <p><strong>Provider:</strong> <?php print check_plain($existing_screening['provider']); ?></p>
    </div>
    <div class="screening-actions">
        <button class="button sims-ai-screen-button" data-node-id="<?php print $node->nid; ?>">
        Refresh Analysis
        </button>
    </div>
    </div>
<?php else: ?>
    <div class="screener-placeholder">
    <div class="placeholder-icon">🤖</div>
    <p>No screening yet. Click the button above to start AI analysis.</p>
    </div>
<?php endif; ?>
</div>

<div class="screener-tips">
<h4>💡 What the AI analyzes:</h4>
<ul>
    <li>Problem statement clarity and significance</li>
    <li>Market opportunity size and validity</li>
    <li>Revenue model feasibility</li>
    <li>Overall pitch quality and completeness</li>
    <li>Key strengths and areas for improvement</li>
</ul>
</div>
</div>