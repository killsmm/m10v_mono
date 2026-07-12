$(function(){

	var POLLLING_INVERVAL_TIME_IN_MILLIS = 10000;//10s

	(function polling() {
	get_json('NBB');
	get_json('SSC');
	window.setTimeout(polling, POLLLING_INVERVAL_TIME_IN_MILLIS);
	}());

	function get_json(evtype){
		$.ajax({
			url: "/cgi-bin/evrec/get_json.cgi",
			type: "POST",
			dataType: "html",
			contentType: "text/html",
			data: evtype,
			scriptCharset: "utf-8"
		}).done( function(ret){
			if(ret == 'Error'){
				window.alert("error in cgi.");

			}else{
				if(evtype == 'NBB'){
					$('#bad_block').html(ret);

				}else if(evtype == 'SSC'){
					$('#sen_stat').html(ret);

				}else{
					window.alert("Error. Unknow event happened.");
				}
			}
		})
		.fail( function(){
			//window.alert("Error happend. Cannot execute cgi");
		});
	}

});
