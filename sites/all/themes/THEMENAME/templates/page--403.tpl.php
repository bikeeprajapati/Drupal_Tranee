<?php print $page_top; ?>
<?php print render($page['navigation']); ?>
<div class="container" style="margin-top:80px;margin-bottom:80px;">
  <div style="max-width:520px;margin:0 auto;background:#fff;border-radius:20px;padding:60px 50px;box-shadow:0 8px 40px rgba(0,0,0,0.1);text-align:center;">
    <div style="width:90px;height:90px;background:linear-gradient(135deg,#fee2e2,#fecaca);border-radius:50%;display:flex;align-items:center;justify-content:center;margin:0 auto 28px auto;font-size:40px;">🔒</div>
    <div style="font-size:72px;font-weight:800;background:linear-gradient(135deg,#667eea,#764ba2);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;line-height:1;margin-bottom:16px;">403</div>
    <h1 style="font-size:26px;font-weight:800;color:#1f2937;margin:0 0 12px 0;">Access Denied</h1>
    <p style="color:#6b7280;font-size:15px;line-height:1.7;margin-bottom:32px;">You don't have permission to access this page. Please log in with the correct account or contact your administrator.</p>
    <div style="display:flex;gap:12px;justify-content:center;flex-wrap:wrap;">
      <a href="/drupal7/" style="background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;padding:12px 28px;border-radius:10px;text-decoration:none;font-weight:700;font-size:15px;">Go Home</a>
      <a href="/drupal7/?q=user/login" style="background:#f3f4f6;color:#374151;padding:12px 28px;border-radius:10px;text-decoration:none;font-weight:700;font-size:15px;border:2px solid #e5e7eb;">Log In</a>
    </div>
    <div style="margin-top:40px;padding-top:24px;border-top:1px solid #f3f4f6;">
      <p style="color:#9ca3af;font-size:13px;margin:0;">InnovateX Incubator — Role-based access control</p>
    </div>
  </div>
</div>
<?php print $page_bottom; ?>
