// JavaScript Document
var tmp_data;
var isdraw;
var callback;

// -------------------------------------------------------------------
// Get AE table Modal's ID
// -------------------------------------------------------------------
function aetbl_modal_id()
{
    return '#modal_AEMetering_Edit';
}

// -------------------------------------------------------------------
// SetUp AE table Edit Modal
// -------------------------------------------------------------------
function aetbl_create_modal()
{
    // [Yossy memo]
    //  is not cool.
    // I have preserved intentionary the following code.
    // It seem better the modal is loading by jquery.
    // But only, Chrome is not working the load function on local files,
    // then this is unconvenience when debug.
    return '<!-- Modal -->\
            <div class="modal fade" id="modal_AEMetering_Edit" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" >\
                <div class="modal-dialog" style="width:500px; height:500px" role="document">\
                    <div class="modal-content">\
                        <div class="modal-header">\
                            <button type="button" class="close" data-dismiss="modal" aria-label="Close">\
                                <span aria-hidden="true">&times;</span>\
							</button>\
							<h5 class="modal-title" style="width:30%;" id="AEMetering_Edit_title">Modal title</h5>\
                        </div>\
                        <div class="modal-body">\
                            <div aligh="center" >\
                                <div>\
									<div style="width:500px; height:370px">\
										<img id="ae_wait" style="padding-top: 20%; padding-left: 30%" src="images/image_loading.gif"/>\
										<div id="ae_waitLbl" style="margin-top: 0px; padding-left: 30%; margin-left: 45px; color: black">\
											<label>Please wait</label>\
										</div>\
										<table id="tbl1" style="margin-left: 50px"></table>\
									</div>\
                                    <div style="margin-left: 40px;">\
										<span>\
                                        <label>Strong</label>\
                                        <button class="btn btn-primary-sel1 " href="#" role="button" id="button_AEMeteringEdit_Sel1">1</button>\
                                        <button class="btn btn-primary-sel2 " href="#" role="button" id="button_AEMeteringEdit_Sel2">2</button>\
                                        <button class="btn btn-primary-sel3 " href="#" role="button" id="button_AEMeteringEdit_Sel3">3</button>\
                                        <button class="btn btn-primary-sel4 " href="#" role="button" id="button_AEMeteringEdit_Sel4">4</button>\
                                        <button class="btn btn-primary-sel5 " href="#" role="button" id="button_AEMeteringEdit_Sel5">5</button>\
                                        <button class="btn btn-primary-sel6 " href="#" role="button" id="button_AEMeteringEdit_Sel6">6</button>\
                                        <button class="btn btn-primary-sel7 " href="#" role="button" id="button_AEMeteringEdit_Sel7">7</button>\
                                        <button class="btn btn-primary-sel0 " href="#" role="button" id="button_AEMeteringEdit_Sel0">-</button>\
                                        <label>Week</label>\
										</span>\
                                    </div>\
                                </div>\
                            </div>\
                        </div>\
                        <div class="modal-footer">\
                            <button type="button" class="btn btn-primary" id="modal_AEMeteringEdit_Save">Save changes</button>\
                            <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>\
                        </div>\
                    </div>\
                </div>\
            </div>';
}

// -------------------------------------------------------------------
// Paint modal's background
// -------------------------------------------------------------------
function aetbl_paint_background()
{
	var ae_metering_mode_text = $('#select_AE_Default_AEMetering option:selected').text();
	var table = $('<table></table>');

	for(var i = 0; i < 32; i++)
	{
		var row = $('<tr></tr>');
		for (j = 0; j < 32; j++)
		{
			row.append($("<td></td>"));
		}
		table.append(row);
	}
	$('#tbl1').empty();
	$('#tbl1').append(table);

	$('#tbl1 td').click(function()
	{
        aetbl_change_handler(this);
	});
    $('#tbl1 td').on('mousedown', function(e)
    {
		isdraw = true;
		aetbl_change_handler(this);
    });
    $('#tbl1 td').on('mouseup', function(e)
    {
		isdraw = false;
        aetbl_change_handler(this);
    });
    $('#tbl1 td').on('mousemove', function(e)
    {
		if (isdraw)
		{
	        aetbl_change_handler(this);
		}
    });

	$("#AEMetering_Edit_title").text(ae_metering_mode_text);
}

// -------------------------------------------------------------------
// Change AE weight handling
// -------------------------------------------------------------------
function aetbl_change_handler(ui)
{
	var row_index = $(ui).parent().index();
	var col_index = $(ui).index();

	$(ui).css("background-color", tmp_data.ae_weight_color[tmp_data.ae_metering["weight"]]);
	tmp_data.ae_weight[row_index][col_index] = tmp_data.ae_metering["weight"];
}

// -------------------------------------------------------------------
// Draw contents
// -------------------------------------------------------------------
function aetbl_render()
{
	if (tmp_data.ae_weight != undefined)
	{
		for (var i = 0; i < 32; i++)
		{
			for (var j = 0; j < 32; j++)
			{
				var tr = $('#tbl1 tr').eq(i);
				var td = tr.children("td").eq(j);
				td.css("background-color", tmp_data.ae_weight_color[tmp_data.ae_weight[i][j]]);
			}
		}
	}
}

// -------------------------------------------------------------------
// Show modal
// -------------------------------------------------------------------
function aetbl_show_modal(param, func)
{
	tmp_data = 
	{
		ae_metering : {
			"weight" : 0
		},
		ae_weight_color : [
			"#fff",
			"#e00",
			"#ec0",
			"#ff0",
			"#8f0",
			"#8ff",
			"#88f",
			"#22f"
		],
		ae_weight : [[]]
	};

    callback = func;

	isdraw = false;

	for (var i = 0; i < 32; i++)
	{
		tmp_data.ae_weight[i] = [];

		for (var j = 0; j < 32; j++)
		{
			tmp_data.ae_weight[i][j] = 0;
		}
	}
		
	aetbl_set_waitarea(true);
    aetbl_paint_background();
    aetbl_render();
	aetbl_update_button();
	$(aetbl_modal_id()).modal();

	// Request getting AE weight to RTOS
	rtos_get_aeweight(param, function(data)
	{
		aetbl_set_waitarea(false);

		for (var i = 0; i < 32; i++)
		{
			tmp_data.ae_weight[i] = [];

			for (var j = 0; j < 32; j++)
			{
				tmp_data.ae_weight[i][j] = data.AE.ae_weight[i*32+j];
			}
		}

		aetbl_paint_background();
		aetbl_render();
	});	
}

// -------------------------------------------------------------------
// Update button style
// -------------------------------------------------------------------
function aetbl_update_button()
{
	var tar = tmp_data.ae_metering["weight"];
	var uiList = [
		'#button_AEMeteringEdit_Sel0',
		'#button_AEMeteringEdit_Sel1',
		'#button_AEMeteringEdit_Sel2',
		'#button_AEMeteringEdit_Sel3',
		'#button_AEMeteringEdit_Sel4',
		'#button_AEMeteringEdit_Sel5',
		'#button_AEMeteringEdit_Sel6',
		'#button_AEMeteringEdit_Sel7'
	];
	
	uiList.forEach(function(val, index, array)
	{
		$(val).css("border-color", tar == index ? "#DD0" : "#333");
	});
}

// -------------------------------------------------------------------
// Initialie modal
// -------------------------------------------------------------------
function aetbl_init()
{
    $('#button_AEMeteringEdit_Sel0').on('click', function()
	{
		tmp_data.ae_metering["weight"] = 0;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel1').on('click', function()
	{
		tmp_data.ae_metering["weight"] = 1;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel2').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 2;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel3').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 3;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel4').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 4;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel5').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 5;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel6').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 6;
		aetbl_update_button();
    });
    $('#button_AEMeteringEdit_Sel7').on('click', function() 
	{
		tmp_data.ae_metering["weight"] = 7;
		aetbl_update_button();
    });
	$('#modal_AEMeteringEdit_Save').click(function()
	{
		var param = {};
		var ae_metering_mode = $('#select_AE_Default_AEMetering').val();

		var data = {};
		data.ae_weight = [];
		for (var i = 0; i < 32; i++)
		{
			for (var j = 0; j < 32; j++)
			{
				data.ae_weight[i * 32 + j] = tmp_data.ae_weight[i][j];
			}
		}
 
		param = app_get_rtos_args();
		param.camera_mode = app_check_disable() ? 1 : 2;
		param.ae_table_pos = 16 + 32 / 2 * 32 * ae_metering_mode;
		param.set_type = "AEMetering_Edit";
		param.set_data = data;

		aetbl_set_waitarea(true);

		// Request setting AE weight to RTOS
		rtos_set_aeweight(param, function(data)
		{
			aetbl_set_waitarea(false);

			window.alert("Done.");
		});
	});
    $(aetbl_modal_id()).on('hidden.bs.modal', function()
    {
		if (callback != undefined)
		{
	        callback(tmp_data);
		}
    });
}

// -------------------------------------------------------------------
// Set Wait area visiblity
// -------------------------------------------------------------------
function aetbl_set_waitarea(visible)
{
	if (visible)
	{
		$("#tbl1").css("display", "none");
		$("#ae_wait").css("display", "");
		$("#ae_waitLbl").css("display", "");
	}
	else
	{
		$("#tbl1").css("display", "");
		$("#ae_wait").css("display", "none");
		$("#ae_waitLbl").css("display", "none");
	}

	return;
}