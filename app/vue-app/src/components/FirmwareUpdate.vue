<template>

  <el-divider />
  <h4>Firmware update</h4>
  <el-upload
    ref="upload"
    class="upload-demo"
    action="http://192.168.137.16:8080/api/camera/update"
    :limit="1"
    :on-exceed="handleExceed"
    :on-progress="handleProgress"
    :on-error="handleError"
    :on-success="handleSuccess"
    :auto-upload="false"
  >
    
    <template #trigger>
    <div>
      <el-button class="ml-3" type="primary">select file</el-button>
    </div>
      
    </template>
    <el-button class="ml-3" type="success" @click="submitUpload">
      update
    </el-button>
    <template #tip>
      <div class="el-upload__tip text-red">
        Please select firmware package *.tar
      </div>
    </template>
  </el-upload>
</template>

<script>
import { ref } from 'vue'
export default {
    setup() {
        let bodyFormData = new FormData();
        bodyFormData.append('userName', 'Fred');


        const upload = ref(null)
        const handleExceed = (files) => {
            console.log("exceed")
            upload.value.clearFiles()
            const file = files[0]
            upload.value.handleStart(file)
        }

        const submitUpload = () => {
            console.log("start upload")
            // upload.value.clearFiles()
            upload.value.submit()
            // upload.value!.submit()
        }

        const handleProgress = () => {
          console.log("progress")
        }

        const handleError = () => {
          alert("update failed")
        }

        const handleSuccess = (response) => {
          console.log(response)
          alert("update " + response.result )
        }

        return {upload, handleExceed, submitUpload, handleProgress, handleError, handleSuccess}
    }
}
</script>

<style lang="css" scoped>
.el-button {
  padding: auto;
  margin: 10px;
}
</style>