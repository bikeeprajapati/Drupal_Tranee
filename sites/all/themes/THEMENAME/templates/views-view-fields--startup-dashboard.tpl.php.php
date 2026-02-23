<?php
/**
 * @file
 * Responsive card template for InnovateX Startup Dashboard.
 */
?>

<div class="startup-dashboard-grid">
<div class="card shadow-sm">
<div class="card-body">
    <h5 class="card-title"><?php print $fields['title']->content; ?></h5>
    <p class="card-text"><strong>Submitted By:</strong> <?php print $fields['name']->content; ?></p>
    <p class="card-text"><strong>Industry:</strong> <?php print $fields['field_industry']->content; ?></p>
    <p class="card-text"><strong>Cohort:</strong> <?php print $fields['field_cohort']->content; ?></p>
    <p class="card-text"><strong>Funding Requested:</strong> $<?php print $fields['field_funding_requested']->content; ?></p>
    <p class="card-text">
    <strong>Status:</strong>
    <span class="badge status-<?php print strtolower($fields['field_startup_status']->raw); ?>">
        <?php print $fields['field_startup_status']->content; ?>
    </span>
    </p>
</div>
</div>
</div>
