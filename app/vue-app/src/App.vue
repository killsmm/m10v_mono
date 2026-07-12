<template>
  <el-tabs v-model="activeName" type="border-card" class="demo-tabs">
    <el-tab-pane label="Preview" name="first">
      <h2>Preview</h2>
      <ImagePreviewer></ImagePreviewer>
    </el-tab-pane>
    <el-tab-pane label="Playback" name="third">
      <h2>Playback</h2>
    </el-tab-pane>
    <el-tab-pane label="Settings" name="second">
      <h2>Settings</h2>
      <FirmwareUpdate></FirmwareUpdate>
    </el-tab-pane>
    <el-tab-pane label="API" name="fourth">
      <h2>API</h2>
      <CameraController></CameraController>
    </el-tab-pane>
    <el-tab-pane label="Work Info" name="fifth">
      <h2>Work Info</h2>
      <WorkInfoDisplay></WorkInfoDisplay>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
import ImagePreviewer from './components/ImagePreviewer.vue';
import CameraController from './components/CameraController.vue';
import FirmwareUpdate from './components/FirmwareUpdate.vue';
import WorkInfoDisplay from './components/WorkInfoDisplay.vue';
import { ref } from 'vue';

export default {
  
  name: 'App',
  components: {
    ImagePreviewer,
    CameraController,
    FirmwareUpdate,
    WorkInfoDisplay
  },
  setup() {
    let activeName = ref('first')
    return {
      activeName,
    }
  },
}
</script>

<style>

#app {
  font-family: Segoe UI, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  margin-top: 60px;
}

.demo-tabs > .el-tabs__content {
  padding: 32px;
  color: #6b778c;
  font-size: 32px;
  font-weight: 100;
}

</style>
