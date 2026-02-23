(function($){Drupal.behaviors.progressTracker={attach:function(context,settings){$('#edit-completion-percentage',context).once('progress-auto-status').change(function(){var percentage=$(this).val();
var statusField=$('#edit-status');
if(percentage==0){statusField.val('not_started')}else if(percentage==100){statusField.val('completed');$('.form-item-actual-date').slideDown()}else{statusField.val('in_progress');
$('.form-item-actual-date').slideUp()}});
$('.button-delete',context).once('confirm-delete').click(function(e){if(!confirm('Are you sure?')){e.preventDefault();
return false}});$('.progress-fill',context).once('progress-animate').each(function(){var width=$(this).css('width');
$(this).css('width','0');
setTimeout(function(elem){$(elem).css('width',width)},100,this)})}}})(jQuery);
