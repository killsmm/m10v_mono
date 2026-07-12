<template>
    <div>
        <!-- <h1>CameraController</h1> -->

        <p>{{ api_url }}</p>
        <!-- <el-row class="mb-4"> -->
        <div class="flex_container">
            <select class="flex_auto" name="api" id="api" v-model="api_url">
                <option value="http://192.168.137.16:8080/api/camera/take_photo">take photo</option>
                <option value="http://192.168.137.16:8080/api/camera/company_infocompany_name">company info</option>
                <option value="http://192.168.137.16:8080/api/camera/shutter_mode">shutter_mode</option>
                <option value="http://192.168.137.16:8080/api/camera/photo_resolve">photo_resolution</option>
                <option value="http://192.168.137.16:8080/api/camera/photo_quality">photo photo_quality</option>
                <option value="http://192.168.137.16:8080/api/camera/digital_zoom">digital zoom</option>
                <option value="http://www.google.com">google</option>
            </select>
            <el-button class="flex_auto" type="primary" @click="get">get</el-button>
            <el-button class="flex_auto" type="primary" @click="post">post</el-button>
            <!-- four direction buttons -->
            <!-- <el-button class="direction" type="primary" @click="get">up</el-button>
            <el-button class="direction" type="primary" @click="get">down</el-button>
            <el-button class="direction" type="primary" @click="get">left</el-button>
            <el-button class="direction" type="primary" @click="get">right</el-button> -->
            <VueH5sPlayer></VueH5sPlayer>
        </div>
        <!-- </el-row> -->


    </div>
</template>

<script>
import axios from "axios"
import VueH5sPlayer from 'vue-h5s-player'
const header = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
}
const data = {
    email: "aa",
    name: "zhouxinliang"
}
export default {
    name: 'CameraController',
    components: {
        VueH5sPlayer
    },
    data() {
        return {
            api_url: '192.168.137.16'
        }
    },
    methods: {
        get() {
            axios
                .get(this.api_url, data, { headers: header })
                .then(response => (console.log(response)))
                .catch(error => console.log(error))
        },
        post() {
            axios.post(this.api_url, data, { headers: header })
                .then(response => (console.log(response)))
                .catch(error => console.log(error))
        },

    },
}
</script>

<style lang="css" scoped>
    .flex_container{
        display: flex;
    };
    .flex_auto{
        flex: auto;
        margin-right: 100px;
    };
    select{
        flex: auto;
        margin-right: 100px;
    }
    .direction{
        flex: 1 1 0;
    }
</style>