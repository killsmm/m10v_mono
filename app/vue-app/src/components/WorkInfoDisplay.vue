<template>
    <div>
        <div class="flex_container">
            <img :src="battery">
            <div>
                <h4>Battery:{{ battery_info }}</h4>
                <h4>Level:  {{ battery_level }}</h4>
            </div>
            <div class="flex_container">
                <img :src="sd_card">
                <div>
                    <h4>Storage:{{ sd_card_storage }}</h4>
                    <h4>Free:   {{ sd_card_free }}</h4>
                </div>
            </div>
        </div>
        <el>
            <li v-for="(value, name) in info" :key="name">
                {{ name }}: {{ value }}
            </li>
            <el-button class="flex_auto" type="primary" @click="get">get</el-button>
        </el>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import axios from "axios"
export default {
    setup() {
        // const parseInfo = (json_info) => {
        //     /* parse the json info */
        //     let info = JSON.parse(json_info)
        //     return info
        // }


        let info = ref({"sdcard_info": "123123123", "battery_info": "123", "shutter_mode": "123", "photo_resolution": "123", "photo_quality": "123", "digital_zoom": "123"})
        const get = () => {
            axios.get(`api/camera/work_info`).then(function (response) {
                console.log(response)
                info.value = response.data
            }).catch(error => console.log(error))
        }
        onMounted(() => {
            get()
            
        })

        const sd_card = require("@/assets/sd_card.png")
        const battery = require("@/assets/battery.png")

        return {info, get, sd_card, battery, sd_card_storage: info.value.sdcard_info, sd_card_free: info.value.sdcard_info}
    }
}
</script>

<style lang="css" scoped>
li {
    align-items: center;
    margin-bottom: 10px;
    font-size: 3vh
}
.flex_container{
    display: flex;
    align-items: center;
    margin-bottom: 10px;
    font-size: 3vh
}

h4{
    margin-left: 10px;
    margin-right: 10px;
    font-size: 2vh
}

img{
    margin-left: 10px;
    margin-right: 10px;
    width: 10vh;
    height: 10vh;
    object-fit: contain;
}
</style>