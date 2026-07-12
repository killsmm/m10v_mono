# Change Camera Mode -------------------------------------------------
# IPCU Command: Change Camera Mode
# Sub Commnad : Change Mode
# Params      : Idle
camera_if chg_camera_mode 1

# Update module
# IPCU Command: Simple Tool
# Sub Commnad : Update
# Params      : video update
camera_if simple_tool_update 0

# Change Camera Mode -------------------------------------------------
# IPCU Command: Change Camera Mode
# Sub Commnad : Change Mode
# Params      : Still Capture
camera_if chg_camera_mode 2

# IPCU Command: Set Image Configuration
# Sub Commnad : ISO
# Params      : 0x00(Auto)
# TODO        : UIの値が設定されるんだけど、Autoにしたいわけじゃないの？？
camera_if s_i_iso ${3}

# IPCU Command: Set Image Configuration
# Sub Commnad : White Balance (Mode)
# Params      : 0x00(Auto), 256(x1.0), 256(x1.0)
camera_if s_i_white_balance ${4} 256 256
