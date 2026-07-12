// JavaScript Document
var GAMMA_X_MAX = 4095;             // const value
var GAMMA_Y_MAX = 1023;             // const value
var GAMMA_OPERATION_POINT_MAX = 20; // const value
var GAMMA_DEBUG = false;            // const value
var gm_data;
var tmp_data;
var callback;

// -------------------------------------------------------------------
// Get Gamma Modal's ID
// -------------------------------------------------------------------
function gamma_modal_id()
{
    return '#modal_Gamma_Edit';
}

// -------------------------------------------------------------------
// Deep copy to gamma data
// -------------------------------------------------------------------
function gamma_copy_data(data)
{
    var ret = gamma_init_data();

    gamma_copy_data_core('sync', data, ret);
    gamma_copy_data_core('r', data, ret);
    gamma_copy_data_core('g', data, ret);
    gamma_copy_data_core('b', data, ret);
    gamma_copy_data_core('y', data, ret);

    return ret;
}

// -------------------------------------------------------------------
// Deep copy to gamma data (core)
// -------------------------------------------------------------------
function gamma_copy_data_core(component, from, to)
{
    if (from != undefined)
    {
        from[component].opepos.forEach(function(val, index, array)
        {
            to[component].opepos[index] = {
                x: val.x,
                y: val.y
            };
        });
        to[component].edited = from[component].edited;
        from[component].curve.forEach(function(val, index, array)
        {
            to[component].curve[index] = {
                x: val.x,
                y: val.y
            };
        });
        from[component].errpos.forEach(function(val, index, array)
        {
            to[component].errpos[index] = {
                x: val.x,
                y: val.y
            };
        });
    }
}

// -------------------------------------------------------------------
// Convert Rtos command result to UI data
// -------------------------------------------------------------------
function gamma_convert_rtos_result(data)
{
    var param = {};

    if (!(data.data0 != undefined
       && data.data1 != undefined
       && data.data2 != undefined
       && data.data3 != undefined
       && data.data4 != undefined))
    {
        param = {
            "index": data.index,
            "lut": [
                gamma_init_data(),
                gamma_init_data(),
                gamma_init_data(),
                gamma_init_data(),
                gamma_init_data()]
        };
    }
    else
    {
        param = {
            "index": data.index,
            "lut": [
                gamma_convert_rtos_result_core(data.data0),
                gamma_convert_rtos_result_core_with_graphdata(data.data1),
                gamma_convert_rtos_result_core_with_graphdata(data.data2),
                gamma_convert_rtos_result_core_with_graphdata(data.data3),
                gamma_convert_rtos_result_core_with_graphdata(data.data4)]
        };
    }

    return param;
}

// -------------------------------------------------------------------
// Convert Rtos command result to UI data(Core)
// -------------------------------------------------------------------
function gamma_convert_rtos_result_core(data)
{
    var param = gamma_init_data();

    param['r'] = gamma_convert_rtos_result_core2(data.r_opepos);
    param['g'] = gamma_convert_rtos_result_core2(data.g_opepos);
    param['b'] = gamma_convert_rtos_result_core2(data.b_opepos);
    param['y'] = gamma_convert_rtos_result_core2(data.y_opepos);
    param['sync'] = gamma_convert_rtos_result_core2(data.r_opepos);

    return param;
}

// -------------------------------------------------------------------
// Convert Rtos command result to UI data(Core)
// -------------------------------------------------------------------
function gamma_convert_rtos_result_core_with_graphdata(data)
{
    var param = gamma_init_data();

    param['r'] = gamma_convert_rtos_result_core2(data.r_opepos, data.r_full, data.r_diff);
    param['g'] = gamma_convert_rtos_result_core2(data.g_opepos, data.g_full, data.g_diff); 
    param['b'] = gamma_convert_rtos_result_core2(data.b_opepos, data.b_full, data.b_diff);
    param['y'] = gamma_convert_rtos_result_core2(data.y_opepos, data.y_full, data.y_diff);
    param['sync'] = gamma_convert_rtos_result_core2(data.r_opepos, data.r_full, data.r_diff);

    return param;
}

// -------------------------------------------------------------------
// Convert Rtos command result to UI data(Core2)
// -------------------------------------------------------------------
function gamma_convert_rtos_result_core2(opepos, full, diff)
{
    var data = {
        opepos: [],
        edited: false,
        curve: [],
        erropos: []
    };

    if (opepos != undefined)
    {
        for (var i = 0; i < opepos.length; i += 2)
        {
            // Validation
            if (!(opepos[i] >= 0 && opepos[i] <= GAMMA_X_MAX
            && opepos[i + 1] >= 0 && opepos[i + 1] <= GAMMA_Y_MAX))
            {
                break;
            }

            if (i > 0 && (opepos[i - 2] > opepos[i] || opepos[i - 1] > opepos[i + 1]))
            {
                break;
            }

            data.opepos.push({
                x:opepos[i],
                y:opepos[i + 1]
            });
        }
    }

    // Check analysis required
    if (data.opepos.length < 2
   || !(data.opepos[0].x == 0 && data.opepos[data.opepos.length - 1].x == 4095))
    {
        data.opepos = (full != undefined && diff != undefined)
                    ? gamma_analysis_opepos(full, diff)
                    : [{x: 0, y: 0}, {x: GAMMA_X_MAX, y: GAMMA_Y_MAX}];
    }

    return gamma_prepare_spline(data);
}

// -------------------------------------------------------------------
// Analysis operation points from graphdata
// -------------------------------------------------------------------
function gamma_analysis_opepos(full, diff)
{
    var full2 = 0;
    var full4 = 0;
    var full12 = 0;
    var full4095 = 0;

    var ret;

    d2fArray = gamma_analysis_opepos_core(full[0 / 16], diff[0], diff[1]);
    full2 = d2fArray[2];
    full4 = d2fArray[4];
    full12 = d2fArray[12];

    d2fArray = gamma_analysis_opepos_core(full[4080 / 16], diff[510], diff[511]);
    full4095 = d2fArray[15];

    ret = [
        {x: 0, y: full[0 / 16]},
        {x: 2, y: full2},
        {x: 4, y: full4},
        {x: 12, y: full12},
        {x: 16, y: full[16 / 16]},
        {x: 32, y: full[32 / 16]},
        {x: 64, y: full[64 / 16]},
        {x: 128, y: full[128 / 16]},
        {x: 192, y: full[192 / 16]},
        {x: 256, y: full[256 / 16]},
        {x: 384, y: full[384 / 16]},
        {x: 512, y: full[512 / 16]},
        {x: 768, y: full[768 / 16]},
        {x: 1024, y: full[1024 / 16]},
        {x: 1536, y: full[1536 / 16]},
        {x: 2048, y: full[2048 / 16]},
        {x: 2560, y: full[2560 / 16]},
        {x: 3072, y: full[3072 / 16]},
        {x: 4080, y: full[4080 / 16]},
        {x: 4095, y: full4095}            
    ];

    return ret;
}

// -------------------------------------------------------------------
// Analysis operation points from graphdata(COre)
// -------------------------------------------------------------------
function gamma_analysis_opepos_core(full0, diff0, diff1)
{
    var ret = [];

    ret.push(full0);
    ret.push(ret[0] + ((diff0 & 0xF0) >> (4 * 1)));
    ret.push(ret[1] + ((diff0 & 0xF00) >> (4 * 2)));
    ret.push(ret[2] + ((diff0 & 0xF000) >> (4 * 3)));
    ret.push(ret[3] + ((diff0 & 0xF0000) >> (4 * 4)));
    ret.push(ret[4] + ((diff0 & 0xF00000) >> (4 * 5)));
    ret.push(ret[5] + ((diff0 & 0xF000000) >> (4 * 6)));
    ret.push(ret[6] + ((diff0 & 0xF0000000) >> (4 * 7)));
    ret.push(ret[7] + ((diff1 & 0xF) >> (4 * 0)));
    ret.push(ret[8] + ((diff1 & 0xF0) >> (4 * 1)));
    ret.push(ret[9] + ((diff1 & 0xF00) >> (4 * 2)));
    ret.push(ret[10] + ((diff1 & 0xF000) >> (4 * 3)));
    ret.push(ret[11] + ((diff1 & 0xF0000) >> (4 * 4)));
    ret.push(ret[12] + ((diff1 & 0xF00000) >> (4 * 5)));
    ret.push(ret[13] + ((diff1 & 0xF000000) >> (4 * 6)));
    ret.push(ret[14] + ((diff1 & 0xF0000000) >> (4 * 7)));
    
    return ret;
}

// -------------------------------------------------------------------
// Check editted for get Rtos command argument
// -------------------------------------------------------------------
function gamma_get_rtos_argument_check_edit(data, index)
{
    var lut = data["lut"][index];
    
    return !(!lut["sync"].edited
          && !lut["r"].edited
          && !lut["g"].edited
          && !lut["b"].edited
          && !lut["y"].edited);
}

// -------------------------------------------------------------------
// Get Rtos command argument
// -------------------------------------------------------------------
function gamma_get_rtos_argument(data)
{
    var param = {};
    var type = "IQ_gamma";

    // Check editted
    // Ignore index 0, because it's liniear curve.
    if (!gamma_get_rtos_argument_check_edit(data, 1)
     && !gamma_get_rtos_argument_check_edit(data, 2)
     && !gamma_get_rtos_argument_check_edit(data, 3)
     && !gamma_get_rtos_argument_check_edit(data, 4))
    {
        type = "IQ_gamma_index";
    }

    // Create argument
	param = {
        set_type: type,
        set_data: {
            "index": data["index"],
            "data0": gamma_get_rtos_argument_core(data["lut"][0]),
            "data1": gamma_get_rtos_argument_core(data["lut"][1]),
            "data2": gamma_get_rtos_argument_core(data["lut"][2]),
            "data3": gamma_get_rtos_argument_core(data["lut"][3]),
            "data4": gamma_get_rtos_argument_core(data["lut"][4])
        }
    };

    return param;
}

// -------------------------------------------------------------------
// Get Rtos command argument(Core)
// -------------------------------------------------------------------
function gamma_get_rtos_argument_core(data)
{
    var param = {
        "r_full": [],
        "r_diff": [],
        "r_opepos": [],
        "g_full": [],
        "g_diff": [],
        "g_opepos": [],
        "b_full": [],
        "b_diff": [],
        "b_opepos": [],
        "y_full": [],
        "y_diff": [],
        "y_opepos": []
    };
    var fullR;
    var fullG;
    var fullB;
    var fullY;
    var tmpR;
    var tmpG;
    var tmpB;
    var tmpY;

    // Regist operation point data
    for (var i = 0; i < GAMMA_OPERATION_POINT_MAX; i++)
    {
        param["r_opepos"].push(i < data['r'].opepos.length ? data['r'].opepos[i].x : -1);
        param["r_opepos"].push(i < data['r'].opepos.length ? data['r'].opepos[i].y : -1);

        param["g_opepos"].push(i < data['g'].opepos.length ? data['g'].opepos[i].x : -1);
        param["g_opepos"].push(i < data['g'].opepos.length ? data['g'].opepos[i].y : -1);

        param["b_opepos"].push(i < data['b'].opepos.length ? data['b'].opepos[i].x : -1);
        param["b_opepos"].push(i < data['b'].opepos.length ? data['b'].opepos[i].y : -1);

        param["y_opepos"].push(i < data['y'].opepos.length ? data['y'].opepos[i].x : -1);
        param["y_opepos"].push(i < data['y'].opepos.length ? data['y'].opepos[i].y : -1);
    }

    // Regist gamma curve data
    for (var i = 0; i < GAMMA_X_MAX + 1; i++)
    {
        if (i % 16 == 0)
        {
            // Regist full gamma data
            param["r_full"].push(data['r'].curve[i].y);
            param["g_full"].push(data['g'].curve[i].y);
            param["b_full"].push(data['b'].curve[i].y);
            param["y_full"].push(data['y'].curve[i].y);

            tmpR = 0;
            tmpG = 0;
            tmpB = 0;
            tmpY = 0;
        }
        else if (i % 8 == 7)
        {
            tmpR |= gamma_calc_diffdata(i, data['r'].curve[i].y, data['r'].curve[i - 1].y);
            tmpG |= gamma_calc_diffdata(i, data['g'].curve[i].y, data['g'].curve[i - 1].y);
            tmpB |= gamma_calc_diffdata(i, data['b'].curve[i].y, data['b'].curve[i - 1].y);
            tmpY |= gamma_calc_diffdata(i, data['y'].curve[i].y, data['y'].curve[i - 1].y);

            // Regist diff gamma data
            param["r_diff"].push(tmpR);
            param["g_diff"].push(tmpG);
            param["b_diff"].push(tmpB);
            param["y_diff"].push(tmpY);

            tmpR = 0;
            tmpG = 0;
            tmpB = 0;
            tmpY = 0;
        }
        else
        {
            tmpR |= gamma_calc_diffdata(i, data['r'].curve[i].y, data['r'].curve[i - 1].y);
            tmpG |= gamma_calc_diffdata(i, data['g'].curve[i].y, data['g'].curve[i - 1].y);
            tmpB |= gamma_calc_diffdata(i, data['b'].curve[i].y, data['b'].curve[i - 1].y);
            tmpY |= gamma_calc_diffdata(i, data['y'].curve[i].y, data['y'].curve[i - 1].y);
        }
    }

    return param;
}

// -------------------------------------------------------------------
// Calcurate diff data
// -------------------------------------------------------------------
function gamma_calc_diffdata(index, data, preData)
{
    var diff = data - preData;

    return (diff & 0xF) << (4 * (index % 8));
}

// -------------------------------------------------------------------
// Show modal
// -------------------------------------------------------------------
function gamma_show_modal(data, func)
{
	// Check gamma selected
	if (data["index"] != undefined && data["index"] != 0)
	{
        gm_data = data["lut"][data["index"]] != undefined
                ? data["lut"][data["index"]]
                : gamma_init_data();

        // Regist call back function
        callback = func;

        // Set up tmp_data
        tmp_data = {
            current: {
                index: 0,
                component:'r', // Not sync is default, because it is overwrited to updated IQ.bin's settings.
                grab: undefined,
                hit: undefined,
                selected: undefined
            },
            operation: gamma_copy_data(gm_data)
        };

        tmp_data.current.index = parseInt(data["index"]);
        tmp_data.current.grab = undefined;

        gamma_update_button_style(tmp_data.current.component);
        gamma_selection_init(undefined);
	    gamma_paint_background();
       	gamma_render(undefined);
		$(gamma_modal_id()).modal();
	}
	else 
	{
		window.alert("Gamma curve does not select!");
	}
}

// -------------------------------------------------------------------
// Init selected operation point data
// ------------------------------------- ------------------------------
function gamma_selection_init(color_id)
{
    switch (color_id)
    {
        case 'sync':
        case 'r':
        case 'g':
        case 'b':
        case 'y':
            tmp_data.current.selected[color_id] = undefined;
            break;
        default:
            tmp_data.current.selected = {
                'sync': undefined,
                'r': undefined,
                'g': undefined,
                'b': undefined,
                'y': undefined  
            };
            break;
    }
}

// -------------------------------------------------------------------
// Get Gamma curve initialize data
// -------------------------------------------------------------------
function gamma_init_data(index)
{
    var data =  {
        'sync': {
            opepos: gamma_init_data_core(index),
            edited: false,
            curve: [],
            errpos: []
        },
        'r': {
            opepos: gamma_init_data_core(index),
            edited: false,
            curve: [],
            errpos: []
        },
        'g': {
            opepos: gamma_init_data_core(index),
            edited: false,
            curve: [],
            errpos: []
        },
        'b': {
            opepos: gamma_init_data_core(index),
            edited: false,
            curve: [],
            errpos: []
        },
        'y': {
            opepos: gamma_init_data_core(index),
            edited: false,
            curve: [],
            errpos: []
        }
    };

    gamma_prepare_spline(data['sync']);
    gamma_prepare_spline(data['r']);
    gamma_prepare_spline(data['g']);
    gamma_prepare_spline(data['b']);
    gamma_prepare_spline(data['y']);

    return data;
}

// -------------------------------------------------------------------
// Get Gamma curve initialize data(Core)
// -------------------------------------------------------------------
function gamma_init_data_core(index)
{
    var ret = [];
    
    switch (index)
    {
        case 1:
            ret = [ {x: 0, y: 0},
                    {x: 4, y: 22},
                    {x: 12, y: 40},
                    {x: 16, y: 47},
                    {x: 32, y: 69},
                    {x: 64, y: 102},
                    {x: 128, y: 149},
                    {x: 192, y: 187},
                    {x: 256, y: 219},
                    {x: 384, y: 275},
                    {x: 512, y: 322},
                    {x: 768, y: 404},
                    {x: 1024, y: 474},
                    {x: 1536, y: 593},
                    {x: 2048, y: 696},
                    {x: 2560, y: 788},
                    {x: 3072, y: 872},
                    {x: 4080, y: 1021},
                    {x: 4095, y: 1023}];
            break;
        case 2:
            ret = [ {x: 0, y: 0},
                    {x: 4, y: 32},
                    {x: 12, y: 55},
                    {x: 16, y: 64},
                    {x: 32, y: 90},
                    {x: 64, y: 128},
                    {x: 128, y: 181},
                    {x: 192, y: 222},
                    {x: 256, y: 256},
                    {x: 384, y: 313},
                    {x: 512, y: 362},
                    {x: 768, y: 443},
                    {x: 1024, y: 512},
                    {x: 1536, y: 627},
                    {x: 2048, y: 723},
                    {x: 2560, y: 809},
                    {x: 3072, y: 886},
                    {x: 4080, y: 1021},
                    {x: 4095, y: 1023}];
            break;
        case 3:
            ret = [ {x: 0, y: 0},
                    {x: 4, y: 44},
                    {x: 12, y: 72},
                    {x: 16, y: 82},
                    {x: 32, y: 113},
                    {x: 64, y: 155},
                    {x: 128, y: 212},
                    {x: 192, y: 255},
                    {x: 256, y: 290},
                    {x: 384, y: 349},
                    {x: 512, y: 398},
                    {x: 768, y: 478},
                    {x: 1024, y: 545},
                    {x: 1536, y: 655},
                    {x: 2048, y: 747},
                    {x: 2560, y: 826},
                    {x: 3072, y: 898},
                    {x: 4080, y: 1021},
                    {x: 4095, y: 1023}];
            break;
        case 4:
            ret = [ {x: 0, y: 0},
                    {x: 4, y: 4},
                    {x: 12, y: 16},
                    {x: 16, y: 20},
                    {x: 32, y: 40},
                    {x: 64, y: 80},
                    {x: 128, y: 144},
                    {x: 192, y: 201},
                    {x: 256, y: 253},
                    {x: 384, y: 337},
                    {x: 512, y: 405},
                    {x: 768, y: 522},
                    {x: 1024, y: 614},
                    {x: 1536, y: 746},
                    {x: 2048, y: 842},
                    {x: 2560, y: 911},
                    {x: 3072, y: 963},
                    {x: 4080, y: 1023},
                    {x: 4095, y: 1023}];
            break;
        default:
            ret = [{x: 0, y: 0}, {x: 4095, y: 1023}];
            break;
    }

    return ret;
}

// -------------------------------------------------------------------
// SetUp Gamma Edit Modal
// -------------------------------------------------------------------
function gamma_create_modal()
{
    // [Yossy memo]
    //  is not cool.
    // I have preserved intentionary the following code.
    // It seem better the modal is loading by jquery.
    // But only, Chrome is not working the load function on local files,
    // then this is unconvenience when debug.
    return '<!-- Modal -->\
            <div class="modal fade" id="modal_Gamma_Edit" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" >\
                <div class="modal-dialog" style="width:600px; height:500px" role="document">\
                    <div class="modal-content">\
                        <div class="modal-header">\
                            <button type="button" class="close" data-dismiss="modal" aria-label="Close">\
                                <span aria-hidden="true">&times;</span>\
                            </button>\
                            <h5 class="modal-title" style="width:30%;" >Gamma Editor</h5>\
                        </div>\
                        <div class="modal-body">\
                            <div aligh="center" >\
                                <button class="btn btn-gamma" type="sync" href="#" role="button" id="button_Gamma_Edit_Sync">Sync Tuning</button>\
                                <button class="btn btn-gamma" type="r" href="#" role="button" id="button_Gamma_Edit_SelectR">Red</button>\
                                <button class="btn btn-gamma" type="g" href="#" role="button" id="button_Gamma_Edit_SelectG">Green</button>\
                                <button class="btn btn-gamma" type="b" href="#" role="button" id="button_Gamma_Edit_SelectB">Blue</button>\
                                <button class="btn btn-gamma" type="y" href="#" role="button" id="button_Gamma_Edit_SelectY">Y</button>\
                                <label>Edit:(x, y) = (</label>\
                                <input id="text_Gamma_Edit_x" type="text" size="3" min="0" max="4095" value="---">\
                                <label>, </label>\
                                <input id="text_Gamma_Edit_y" type="text" size="3" min="0" max="1023" value="---">\
                                <label>)</label>\
                            </div>\
                            <!-- for DEBUG -->\
                            <div align="right">\
                                <input id="debug_tension" type="text" size="3" value="0.25">\
                                <input id="debug_edited" type="text" size="3" value="false">\
                            </div>\
                            <!-- end DEBUG -->\
                            <div aligh="center" >\
                                <canvas id="canvas_Gamma" width="560" height="320"></canvas>\
                            </div>\
                            <div align="left">\
                                <button class="btn btn-primary" href="#" role="button" id="button_Gamma_Edit_Delete">Delete</button>\
                                <button class="btn btn-primary" href="#" role="button" id="button_Gamma_Edit_AllDelete">All Delete</button>\
                            </div>\
                        </div>\
                        <div class="modal-footer">\
                            <button type="button" class="btn btn-primary" id="button_Gamma_Edit_Save">Save changes</button>\
                            <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>\
                        </div>\
                    </div>\
                </div>\
            </div>';
}

// -------------------------------------------------------------------
// Paint modal's background
// -------------------------------------------------------------------
function gamma_paint_background()
{
    var canvas = document.getElementById('canvas_Gamma');
    var ctx = canvas.getContext('2d');
    var region;

    // Draw canvas's backcolor
    ctx.beginPath();
    ctx.fillStyle = 'rgb(245, 245, 245)';
    region = gamma_get_canvas_region();
    ctx.fillRect(
        region.left,
        region.top,
        region.width,
        region.height);

    // Draw Sub-Axisline
    ctx.strokeStyle = 'rgb(150, 150, 150)';
    region = gamma_get_graph_region();
    ctx.beginPath();
    ctx.moveTo(region.left + region.width * 1 / 4, region.top);
    ctx.lineTo(region.left + region.width * 1 / 4, region.bottom);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(region.left + region.width * 2 / 4, region.top);
    ctx.lineTo(region.left + region.width * 2 / 4, region.bottom);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(region.left + region.width * 3 / 4, region.top);
    ctx.lineTo(region.left + region.width * 3 / 4, region.bottom);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(region.left, region.top + region.height * 1 / 4);
    ctx.lineTo(region.right, region.top + region.height * 1 / 4);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(region.left, region.top + region.height * 2 / 4);
    ctx.lineTo(region.right, region.top + region.height * 2 / 4);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(region.left, region.top + region.height * 3 / 4);
    ctx.lineTo(region.right, region.top + region.height * 3 / 4);
    ctx.stroke();

    // Draw Axisline
    ctx.strokeStyle = "black";
    ctx.strokeRect(
        region.left,
        region.top,
        region.width,
        region.height);

    // X Axis label
    ctx.fillStyle = "black";
    ctx.font = "15px 'MS Gothic'";
    ctx.textAlign = "left";
    ctx.textBaseline = "top";
    ctx.fillText('0',    region.left + region.width * 0 / 4 - 15, region.bottom + 10);
    ctx.fillText('1024', region.left + region.width * 1 / 4 - 15, region.bottom + 10);
    ctx.fillText('2048', region.left + region.width * 2 / 4 - 15, region.bottom + 10);
    ctx.fillText('3072', region.left + region.width * 3 / 4 - 15, region.bottom + 10);
    ctx.fillText('4095', region.left + region.width * 4 / 4 - 15, region.bottom + 10);

    // Y Axis label
    ctx.textAlign = "right";
    ctx.fillText('256',  region.left - 10, region.top + region.height * 3 / 4 - 5);
    ctx.fillText('512',  region.left - 10, region.top + region.height * 2 / 4 - 5);
    ctx.fillText('768',  region.left - 10, region.top + region.height * 1 / 4 - 5);
    ctx.fillText('1023', region.left - 10, region.top + region.height * 0 / 4 - 5);
}

// -------------------------------------------------------------------
// Get canvas region
// -------------------------------------------------------------------
function gamma_get_canvas_region()
{
    var canvas = document.getElementById('canvas_Gamma');
    var marginX = 0;
    var marginY = 0;
    var region = {
        left: marginX,
        top: marginY,
        right: canvas.width - marginX,
        bottom: canvas.height - marginY,
        width: canvas.width - marginX * 2,
        height: canvas.height - marginY * 2
    };

    return region;
}

// -------------------------------------------------------------------
// Get graph region
// -------------------------------------------------------------------
function gamma_get_graph_region()
{
    var canvas = document.getElementById('canvas_Gamma');
    var marginX = 50;
    var marginY = 30;
    var region = {
        left: marginX,
        top: marginY,
        right: canvas.width - marginX,
        bottom: canvas.height - marginY,
        width: canvas.width - marginX * 2,
        height: canvas.height - marginY * 2
    };

    return region;
}

// -------------------------------------------------------------------
// Get operation points region
// -------------------------------------------------------------------
function gamma_get_operation_region(pos)
{
    var marginX = 8;
    var marginY = 8;
    var region = {
        left: pos.x - marginX / 2,
        top: pos.y - marginY / 2,
        right: pos.x + marginX / 2,
        bottom: pos.y + marginY / 2,
        width: marginX,
        height: marginY
    };

    return region;
}

// -------------------------------------------------------------------
// Hit test over the specified region
// -------------------------------------------------------------------
function gamma_hit_test(region, e)
{
    var pos;
    var ret = false;

    if (e != undefined)
    {
        pos = gamma_get_mouse_pos(e);
        ret = !(pos.x < region.left || pos.x > region.right || pos.y < region.top || pos.y > region.bottom);
    }

    return ret;
}

// -------------------------------------------------------------------
// Get mouse position
// -------------------------------------------------------------------
function gamma_get_mouse_pos(e)
{
    var rect;
    var pos = {};

    if (e != undefined)
    {
        rect = e.target.getBoundingClientRect();
        pos = {
            x: parseInt(e.clientX - rect.left),
            y: parseInt(e.clientY - rect.top)
        };
    }
    else
    {
        pos = {
            x: '-',
            y: '-'
        }; 
    }

    return pos;
}

// -------------------------------------------------------------------
// Convert canvas position to graph position
// -------------------------------------------------------------------
function gamma_convert_graph_pos(pos)
{
    var region = gamma_get_graph_region();
    var ret = {
        x: 0,
        y: 0
    };

    ret.x = parseInt((pos.x - region.left) * GAMMA_X_MAX / region.width);
    ret.y = parseInt((region.height - (pos.y - region.top)) * GAMMA_Y_MAX / region.height);

    return ret;
}

// -------------------------------------------------------------------
// Convert graph position to canvas position
// -------------------------------------------------------------------
function gamma_convert_canvas_pos(pos)
{
    var region = gamma_get_graph_region();
    var ret = {
        x: 0,
        y: 0
    };

    ret.x = parseInt(pos.x * region.width / GAMMA_X_MAX + region.left);
    ret.y = parseInt(-pos.y * region.height / GAMMA_Y_MAX + region.height + region.top);

    return ret;
}

// -------------------------------------------------------------------
// Prepare gamma cardinal spline curve
// -------------------------------------------------------------------
function gamma_prepare_spline(data)
{
    var points = [];
    var splines = [];
    var pos;
    var prePos;
    var nxtPos;
    var errInfo;
    var refOpePos;
    var opeId = 0;

    // Convert posArray to curve_func argument
    data.opepos.forEach(function(val, index, array)
    {
        pos = val;
        points.push(pos.x);
        points.push(pos.y);
    });

    // Get spline points by curve_func.js
    var tension = parseFloat($('#debug_tension').val());
    splines = getCurvePoints(points, tension, 2048, false);

    // Unzipped result to tmp_data
    data.curve = [];
    data.errpos = [];

    for (var i = 0; i < splines.length; i += 2)
    {
        pos = {};
        pos.x = parseInt(splines[i]);
        pos.y = parseInt(splines[i + 1]);

        // Exclude extra points
        if (i == 0 || pos.x > prePos.x)
        {
            // Get neighbar operation point
            if (opeId < data.opepos.length
             && pos.x == data.opepos[opeId].x)
            {
                pos.y = data.opepos[opeId].y;                
                opeId += (opeId  < data.opepos.length - 1) ? 1 : 0;
                refOpePos = data.opepos[opeId];
            }

            // Update limited position by neighbar operation point
            if (i != 0)
            {
                pos.y = pos.y < prePos.y ? prePos.y : pos.y;                
                pos.y = pos.y > refOpePos.y ? refOpePos.y : pos.y;

                // Check limited position by Degamma macro
                if (pos.x % 16 != 0 && (pos.y - prePos.y > 15))
                {
                    errInfo = {
                        from: prePos,
                        to: pos
                    };
                    data.errpos.push(errInfo);
                }
            }
        
            // Regist gamma curve data
            data.curve.push(pos);
            prePos = pos;
        }

        // Tread horizontal end point
        if (pos.x >= GAMMA_X_MAX)
        {
            break;
        }
    }

    // Interpolate missing point
    for (var i = 1; i < GAMMA_X_MAX + 1; i++)
    {
        prePos = data.curve[i - 1];
        pos = i < data.curve.length ?  data.curve[i] : {};

        // Check missing
        if (pos.x != i)
        {
            var delta = (pos.y - prePos.y) / (pos.x - prePos.x);
            var intercept = prePos.y - delta * prePos.x

            pos = {
                x: i,
                y: parseInt(delta * i + intercept)
            };

            // Add interpolated point
            if (i < data.curve.length)
            {
                data.curve.splice(i, 0, pos);
            }
            else
            {
                data.curve.push(pos);
            }
        }
    }

    return data;
}

// -------------------------------------------------------------------
// Draw gamma cardinal spline curve
// -------------------------------------------------------------------
function gamma_paint_spline(ctx, curve)
{
    var pos;
    var prePos;
    var points = [];

    // Draw
    ctx.beginPath();
    curve.forEach(function(val, index, array)
    {
        pos = gamma_convert_canvas_pos(val);
        ctx.lineTo(pos.x, pos.y);
        
        if (prePos == undefined || prePos.x < pos.x)
        {
            points.push(pos);
            ctx.lineTo(pos.x, pos.y);
            prePos = pos;
        }
    });

    ctx.stroke();
}

// -------------------------------------------------------------------
// Draw error point
// -------------------------------------------------------------------
function gamma_paint_error_point(ctx, errPos)
{
    var fromPos = gamma_convert_canvas_pos(errPos.from);
    var toPos = gamma_convert_canvas_pos(errPos.to);
    var center;
    var distance;
    var rotate;
    var delta;

    delta = {
        x: (toPos.x - fromPos.x) / 2,
        y: (fromPos.y - toPos.y) / 2
    };

    center = {
        x: fromPos.x + delta.x,
        y: toPos.y + delta.y
    };

    distance = Math.sqrt(Math.pow(delta.x, 2) + Math.pow(delta.y, 2));
    rotate = Math.atan(delta.y / delta.x)

    // Draw
    ctx.beginPath();
    ctx.ellipse(center.x, center.y, distance, distance / 4, rotate, 0, Math.PI * 2, false);
    ctx.fill();
    ctx.stroke();
}

// -------------------------------------------------------------------
// Draw operation point
// -------------------------------------------------------------------
function gamma_paint_operation_point(ctx, pos, e, index)
{
    var region = gamma_get_operation_region(gamma_convert_canvas_pos(pos));
    var fStyleBak = ctx.fillStyle;
    var sStyleBak = ctx.strokeStyle;
    var lWidthBak = ctx.lineWidth;

    // Check hit test
    if (e != undefined && gamma_hit_test(region, e))
    {
        tmp_data.current.hit = index;
        ctx.fillStyle = "yellow";
    }

    // Draw operation point
    ctx.fillRect(
        region.left,
        region.top,
        region.width,
        region.height);

    // Treat selection
    if (tmp_data.current.selected[tmp_data.current.component] == index)
    {
        ctx.lineWidth = 4;
        ctx.strokeStyle = "yellow";
        ctx.strokeRect(
            region.left,
            region.top,
            region.width,
            region.height);
    }

    ctx.fillStyle = fStyleBak;
    ctx.strokeStyle = sStyleBak;
    ctx.lineWidth = lWidthBak;
    ctx.strokeRect(
        region.left,
        region.top,
        region.width,
        region.height);

    ctx.fillStyle = fStyleBak;
    ctx.strokeStyle = sStyleBak;
    ctx.lineWidth = lWidthBak;
}

// -------------------------------------------------------------------
// Add operation point
// -------------------------------------------------------------------
function gamma_add_point(e)
{
    var curData = tmp_data.operation[tmp_data.current.component];
    var pos = gamma_convert_graph_pos(gamma_get_mouse_pos(e));
    var region;
    var fromPos;
    var toPos;
    var insertIdx = 0;
    var hit = true;
    var ret = false;

    // Check allow points
    if (ret = curData.opepos.length < GAMMA_OPERATION_POINT_MAX)
    {
        // Check hit test on exist point
        curData.opepos.forEach(function(val, index, array)
        {
            if (hit)
            {
                // Check neighbar point
                if (index - 1 > 0)
                {
                    fromPos = gamma_convert_canvas_pos(array[index - 1]);
                    toPos = gamma_convert_canvas_pos(array[index]);
                }
                else
                {
                    fromPos = gamma_convert_canvas_pos({
                        x: 0, 
                        y: 0
                    });
                    toPos = gamma_convert_canvas_pos(array[index]);
                }

                region = {
                    left: fromPos.x,
                    top: toPos.y,
                    right: toPos.x,
                    bottom: fromPos.y,
                    width: toPos.x - fromPos.x,
                    height: fromPos.y - toPos.y
                };

                if (gamma_hit_test(region, e))
                {
                    // Check hit test
                    hit = gamma_hit_test(gamma_get_operation_region(gamma_convert_canvas_pos(val), e));
                }
            }
        });

        if (!hit)
        {
            // Add point
            curData.opepos.push(pos);
            curData.opepos.sort(function(pre, cur)
            {
                return pre.x > cur.x ? 1 : -1;
            });

            curData.edited = true;
        }
    }

    return ret;
}

// -------------------------------------------------------------------
// Grab operation point
// -------------------------------------------------------------------
function gamma_grab_point(e)
{
    var grabData;
    var curData = tmp_data.operation[tmp_data.current.component];

    // Check hit test on exist point
    curData.opepos.forEach(function(val, index, array)
    {
        if (gamma_hit_test(gamma_get_operation_region(gamma_convert_canvas_pos(val)), e))
        {
            tmp_data.current.grab = index;
        }
    });
}

// -------------------------------------------------------------------
// Relase grabed operation point
// -------------------------------------------------------------------
function gamma_release_point()
{
    tmp_data.current.grab = undefined;
}

// -------------------------------------------------------------------
// Update contents
// -------------------------------------------------------------------
function gamma_update_modal(e, edited)
{
    var syncData = tmp_data.operation['sync'];
    var rData = tmp_data.operation['r'];
    var gData = tmp_data.operation['g'];
    var bData = tmp_data.operation['b'];
    var yData = tmp_data.operation['y'];
    var dRet = true;

    // Treat sync
    if (tmp_data.current.component == 'sync')
    {
        rData.opepos = syncData.opepos.slice(0);
        gData.opepos = syncData.opepos.slice(0);
        bData.opepos = syncData.opepos.slice(0);
        yData.opepos = syncData.opepos.slice(0);

        tmp_data.current.selected['r'] = tmp_data.current.selected['sync'];
        tmp_data.current.selected['g'] = tmp_data.current.selected['sync'];
        tmp_data.current.selected['b'] = tmp_data.current.selected['sync'];
        tmp_data.current.selected['y'] = tmp_data.current.selected['sync'];

        if (edited)
        {
            syncData.edited = true;
            rData.edited = true;
            gData.edited = true;
            bData.edited = true;
            yData.edited = true;
        }

        // Redraw
        gamma_render(e);

        rData.curve = syncData.curve.slice(0);
        gData.curve = syncData.curve.slice(0);
        bData.curve = syncData.curve.slice(0);
        yData.curve = syncData.curve.slice(0);

        rData.errpos = syncData.errpos.slice(0);
        gData.errpos = syncData.errpos.slice(0);
        bData.errpos = syncData.errpos.slice(0);
        yData.errpos = syncData.errpos.slice(0);
    }
    else
    {
        if (edited)
        {
            tmp_data.operation[tmp_data.current.component].edited = true;
        }

        // Redraw
        gamma_render(e);
    }
}

// -------------------------------------------------------------------
// Draw contents
// -------------------------------------------------------------------
function gamma_render(e)
{
    var curData = tmp_data.operation[tmp_data.current.component];
    var canvas = document.getElementById('canvas_Gamma');
    var ctx = canvas.getContext('2d');
    var pos = {
        x: '-',
        y: '-'
    };
    var region;
    var posMsg;

    // Prepare the drawing items
    if (e != undefined && gamma_hit_test(gamma_get_graph_region(), e))
    {
        pos = gamma_convert_graph_pos(gamma_get_mouse_pos(e));
    }

    gamma_prepare_spline(curData);

    // Draw background
    gamma_paint_background();

    // Draw error point
    ctx.fillStyle = "yellow";
    ctx.strokeStyle = "gray";
    curData.errpos.forEach(function(val, index, array)
    {
        gamma_paint_error_point(ctx, val);
    });

    // Draw spline curve
    switch (tmp_data.current.component)
    {
        case 'r':
            ctx.strokeStyle = "red";
            break;
        case 'g':
            ctx.strokeStyle = "green";
            break;
        case 'b':
            ctx.strokeStyle = "blue";
            break;
        case 'y':
            ctx.strokeStyle = "gray";
            break;
        default:
            ctx.strokeStyle = "purple";
            break;
    }

    gamma_paint_spline(ctx, curData.curve);

    // Draw operation point  
    ctx.fillStyle = ctx.strokeStyle;
    ctx.strokeStyle = "black";
    tmp_data.current.hit = undefined;
    curData.opepos.forEach(function(val, index, array)
    {
        gamma_paint_operation_point(ctx, val, e, index);
    });

    // Draw mouse info
    ctx.fillStyle = "black";
    ctx.font = "15px 'MS Gothic'";
    ctx.textAlign = "left";
    ctx.textBaseline = "top";

    if (tmp_data.current.hit != undefined)
    {
        pos = curData.opepos[tmp_data.current.hit];
    }

    if (tmp_data.current.selected[tmp_data.current.component] != undefined)
    {
        pos = curData.opepos[tmp_data.current.selected[tmp_data.current.component]];
    }

    if (pos.x != $("#text_Gamma_Edit_x").val()
     && pos.y != $("#text_Gamma_Edit_y").val())
    {
        $("#text_Gamma_Edit_x").val(pos.x);
        $("#text_Gamma_Edit_y").val(pos.y);
    }
    // For Debug
    ctx.fillStyle = "red";

    if (curData.errpos.length > 0)
    {
        region = gamma_get_canvas_region();
        ctx.fillText("Illegal setting is contained.", region.width * 3/ 5, 10);
    }

    if (GAMMA_DEBUG)
    {
        $('#debug_edited').val(curData.edited);
    }
}

// -------------------------------------------------------------------
// Update color button style
// -------------------------------------------------------------------
function gamma_update_button_style(color_id)
{
    // Update button style
    $('#button_Gamma_Edit_SelectR').removeAttr('type');
    $('#button_Gamma_Edit_SelectG').removeAttr('type');
    $('#button_Gamma_Edit_SelectB').removeAttr('type');
    $('#button_Gamma_Edit_SelectY').removeAttr('type');
    $('#button_Gamma_Edit_Sync').removeAttr('type');

    switch (color_id)
    {
        case 'r':
            $('#button_Gamma_Edit_SelectR').attr('type', color_id);
            break;
        case 'g':
            $('#button_Gamma_Edit_SelectG').attr('type', color_id);
            break;
        case 'b':
            $('#button_Gamma_Edit_SelectB').attr('type', color_id);
            break;
        case 'y':
            $('#button_Gamma_Edit_SelectY').attr('type', color_id);
            break;
        default:
            $('#button_Gamma_Edit_Sync').attr('type', color_id);
            break;
    }
}

// -------------------------------------------------------------------
// Change color component of editting gamma modal
// -------------------------------------------------------------------
function gamma_change_color_component(color_id)
{
    var curData = tmp_data.operation[tmp_data.current.component];
    var syncData = tmp_data.operation['sync'];
    var dRet = true;
    var edited = false;

    if (color_id != tmp_data.current.component)
    {
        // Treat Sync
        if (color_id == 'sync')
        {
            if (dRet = window.confirm('Displayed curve settings copies to other color\'s.'))
            {
                syncData.opepos = curData.opepos.slice(0);
                tmp_data.current.selected['sync'] = tmp_data.current.selected[tmp_data.current.component];

                edited = true;
            }
        }

        if (dRet)
        {
            // Update button style
            gamma_update_button_style(color_id);

            // Chage color component
            tmp_data.current.component = color_id;
            gamma_release_point();
            gamma_update_modal(undefined, edited);
        }
    }
}

// -------------------------------------------------------------------
// Select operation point
// -------------------------------------------------------------------
function gamma_select_point(e)
{
    var curData = tmp_data.operation[tmp_data.current.component];

    // Update grabed operation point
    gamma_selection_init(tmp_data.current.component);

    // Search clicked point
    curData.opepos.forEach(function(val, index, array)
    {
        if (tmp_data.current.selected[tmp_data.current.component] == undefined)
        {
            if (gamma_hit_test(gamma_get_operation_region(gamma_convert_canvas_pos(val)), e))
            {
                tmp_data.current.selected[tmp_data.current.component] = index;
            }
        }
    });
}

// -------------------------------------------------------------------
// Move grabed operation point
// -------------------------------------------------------------------
function gamma_move_grab_point(e)
{
    var curData = tmp_data.operation[tmp_data.current.component];
    var pos = gamma_get_mouse_pos(e);
    var tmpPos;
    var region = gamma_get_graph_region();
    var index = tmp_data.current.grab;

    // Update grabed operation point
    if (tmp_data.current.grab != undefined)
    {
        // Check hit test
        if (gamma_hit_test(gamma_get_canvas_region(), e))
        {
            pos = gamma_convert_graph_pos(pos);

            // Update limited position in the graph region
            if (!gamma_hit_test(gamma_get_graph_region(), e))
            {
                tmpPos = gamma_convert_canvas_pos(pos);
                pos.x = tmpPos.x < region.left ? 0 : pos.x;
                pos.x = tmpPos.x > region.right ? GAMMA_X_MAX : pos.x;
                pos.y = tmpPos.y > region.bottom ? 0 : pos.y;
                pos.y = tmpPos.y < region.top ? GAMMA_Y_MAX : pos.y;
            }

            // Update limited position with neighber points
            if (index - 1 >= 0)
            {
                tmpPos = curData.opepos[index - 1];
                pos.x = tmpPos.x >= pos.x ? tmpPos.x + 1 : pos.x;
                pos.y = tmpPos.y >= pos.y ? tmpPos.y : pos.y;                    
            }

            if (index + 1 <= curData.opepos.length - 1)
            {
                tmpPos = curData.opepos[index + 1];
                pos.x = tmpPos.x <= pos.x ? tmpPos.x - 1 : pos.x;
                pos.y = tmpPos.y <= pos.y ? tmpPos.y : pos.y;                    
            }

            // Update limited position as first data or last data
            if (index == 0 || index == curData.opepos.length - 1)
            {
                // Can be move only y axis
                pos.x = curData.opepos[index].x;
            }
                                 
            curData.opepos[tmp_data.current.grab] = pos;
                
        }
    }
}

// -------------------------------------------------------------------
// Update position of selected operation point
// -------------------------------------------------------------------
function gamma_update_selected_point()
{
    var curData = tmp_data.operation[tmp_data.current.component];
    var x = parseInt($('#text_Gamma_Edit_x').val());
    var y = parseInt($('#text_Gamma_Edit_y').val());
    var index = tmp_data.current.selected[tmp_data.current.component]
    var prePos;
    var proPos;
    var flag = true;

    // Check Selected & inputted number
    if (index != undefined)
    {
        prePos = index > 0 ? curData.opepos[index - 1] : undefined;
        proPos = index < curData.opepos.length ? curData.opepos[index + 1] : undefined;           
        
        // Update fixed value
        if (index == 0)
        {
            x = 0;
        }
        else if (index == curData.opepos.length - 1)
        {
            x = GAMMA_X_MAX;
        }
        
        // Check illegal
        flag = !isNaN(x) && !isNaN(y);

        // Check range
        if (flag)
        {
            flag = !(x < 0 || x > GAMMA_X_MAX)
                && !(y < 0 || y > GAMMA_Y_MAX);
        }

        // Check limited position with neighber operation point
        if (flag && prePos != undefined)
        {
            flag = !(x <= prePos.x || y < prePos.y);
        }

        if (flag && proPos != undefined)
        {
            flag = !(x >= proPos.x || y > proPos.y);
        }

        if (flag)
        {
            // Update edit flag
            if (!(curData.opepos[index].x == x
               && curData.opepos[index].y == y))
            {
                curData.edited = true;
            }
            
            // Update selected operation point
            curData.opepos[index].x = x;
            curData.opepos[index].y = y;
        }
        else
        {
            // redo
            x = curData.opepos[index].x;
            y = curData.opepos[index].y;
        }

        $("#text_Gamma_Edit_x").val(x);
        $("#text_Gamma_Edit_y").val(y);
    }
}

// -------------------------------------------------------------------
// Initialie modal
// -------------------------------------------------------------------
function gamma_init()
{
    var data = 
    {
        "index": 0,
        "lut": [
            gamma_init_data(),
            gamma_init_data(),
            gamma_init_data(),
            gamma_init_data(),
            gamma_init_data()]
    };

    $('#canvas_Gamma').on('click', function(e)
    {
        // Select point
        gamma_select_point(e);

        // Reflesh
        gamma_update_modal(e, false);
    });
    $('#canvas_Gamma').on('dblclick', function(e)
    {
        // Add point
        if (!gamma_add_point(e))
        {
            // Adding point is failed
            window.alert("Up to " + GAMMA_OPERATION_POINT_MAX + " operation points can be arranged.");
        }
        
        // Reflesh
        gamma_update_modal(e, false);
    });
    $('#canvas_Gamma').on('mousedown', function(e)
    {
        // Grab point
        gamma_grab_point(e);

        // Reflesh
        gamma_update_modal(e, false);
    });
    $('#canvas_Gamma').on('mouseup', function(e)
    {
        // Release point
        gamma_release_point(e);

        // Reflesh
        gamma_update_modal(e, false);
    });
    $('#canvas_Gamma').on('mousemove', function(e)
    {
        var edited = false;

        // Update grabed position
        gamma_move_grab_point(e);
        
        edited = tmp_data.current.grab != undefined;

        // Reflesh
        gamma_update_modal(e, edited);
    });
    $('#button_Gamma_Edit_SelectR').on('click', function()
    {
        gamma_change_color_component('r');
    });
    $('#button_Gamma_Edit_SelectG').on('click', function()
    {
        gamma_change_color_component('g');
    });
    $('#button_Gamma_Edit_SelectB').on('click', function()
    {
        gamma_change_color_component('b');
    });
    $('#button_Gamma_Edit_SelectY').on('click', function()
    {
        gamma_change_color_component('y');
    });
    $('#button_Gamma_Edit_Sync').on('click', function()
    {
        gamma_change_color_component('sync');
    });
    $('#button_Gamma_Edit_Save').on('click', function()
    {
        var curData = tmp_data.operation;

        // Check contained error
        if (curData['r'].errpos.length > 0
         || curData['g'].errpos.length > 0
         || curData['b'].errpos.length > 0
         || curData['y'].errpos.length > 0)
        {
            window.alert('This operation can be done when no error contained.');
        }
        else
        {
            window.alert('Done.');
            gm_data = gamma_copy_data(curData);
        }
    });
    $('#button_Gamma_Edit_Delete').on('click', function()
    {
        var curData = tmp_data.operation[tmp_data.current.component];
        var index = tmp_data.current.selected[tmp_data.current.component];

        // Check selected operation point exists
        if (index != undefined && index > 0 && index != curData.opepos.length - 1)
        {
            // Delete selected operation point
            curData.opepos.splice(index, 1);
            gamma_selection_init(tmp_data.current.component);
        }

        // Reflesh
        gamma_update_modal(undefined);
    });
    $('#button_Gamma_Edit_AllDelete').on('click', function()
    {
        var curData = tmp_data.operation[tmp_data.current.component];

        // Delete selected operation point
        curData.opepos = gamma_init_data_core();
        gamma_selection_init(tmp_data.current.component);

        // Reflesh
        gamma_update_modal(undefined);
    });
    $(gamma_modal_id()).on('hidden.bs.modal', function()
    {
        callback(tmp_data.current.index, gm_data);
    });
    $('#text_Gamma_Edit_x').on('keypress', function(e)
    {
        if (e.which == 13)
        {
            gamma_update_selected_point();
            
            // Reflesh
            gamma_update_modal(undefined, false);
        }
    });
    $('#text_Gamma_Edit_x').on('blur', function(e)
    {
        gamma_update_selected_point();
            
        // Reflesh
        gamma_update_modal(undefined, false);
    });
    $('#text_Gamma_Edit_y').on('keypress', function(e)
    {
        if (e.which == 13)
        {
            gamma_update_selected_point();
            
            // Reflesh
            gamma_update_modal(undefined, false);
        }
    });
    $('#text_Gamma_Edit_y').on('blur', function(e)
    {
        gamma_update_selected_point();
            
        // Reflesh
        gamma_update_modal(undefined, false);
    });

    // for Debug
    if (!GAMMA_DEBUG)
    {
        $('#debug_tension').hide();
        $('#debug_edited').hide();
    }

    return data;
}