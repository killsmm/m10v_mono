<template>
    <div id="controller">
        <el-button type="primary" @click=capture>capture</el-button>
        <el-button type="primary" @click=lucky>I'm lucky</el-button>
    </div>
    <h4>Images in RAM</h4>
    <div id="picture_preview">
        <img :src=image_url1 @error=loadDefault alt="tmp1" id="preview_img1">
        <img :src=image_url2 @error=loadDefault alt="tmp2" id="preview_img2">
        <img :src=image_url3 @error=loadDefault alt="tmp3" id="preview_img3">
    </div>
    <h4>Camera Status</h4>
    <div id="information">
        <p>{{ information }}</p>
    </div>
</template>

<script>
import axios from "axios"
import { ref } from 'vue';
export default {
    name: 'ImagePreviewer',
    components: {
    },
    
    setup() {
        const server_ip = window.location.host
        const header = {
            'Content-Type': 'application/json',
            'Access-Control-Allow-Origin': '*',
        }
        const data = {
            email: "aa",
            name: "zhouxinliang"
        }

        const image_url1 = ref(`RAM_DCIM/tmp1.jpg`)
        const image_url2 = ref(`RAM_DCIM/tmp2.jpg`) 
        const image_url3 = ref(`RAM_DCIM/tmp3.jpg`)

        //if images are not exist in the server, set the image url to the default image

        const capture = () =>{
            axios.post(`http://${server_ip}/api/camera/take_photo`, data, { headers: header })
                .then(function (response) {
                    console.log(response)
                    window.location.reload("#preview_img1")
                    window.location.reload("#preview_img2")
                    window.location.reload("#preview_img3")
                })
                .catch(error => console.log(error))
        }

        const getIp = () =>{
            alert(window.location.host)
        }

        const loadDefault = (event) =>{
            //get the element and set the src to the default image
            let id = event.target.id
            let img = document.getElementById(id)
            img.src = require("@/assets/default.png")
        }

        const information = ref('unknown')

        const mounted = () =>{
            axios.get(`api/camera/work_info`).then(function (response) {
                        console.log(response)
                        information.value = response.data
            }).catch(error => console.log(error))
        }

        const lucky = () =>{
            axios.get(`https://dog.ceo/api/breeds/image/random`).then(function (response) {
                        console.log(response)
                        image_url1.value = response.data.message
            }).catch(error => console.log(error))
            axios.get(`https://dog.ceo/api/breeds/image/random`).then(function (response) {
                        console.log(response)
                        image_url2.value = response.data.message
            }).catch(error => console.log(error))
            axios.get(`https://dog.ceo/api/breeds/image/random`).then(function (response) {
                        console.log(response)
                        image_url3.value = response.data.message
            }).catch(error => console.log(error))
        }

        return {capture, loadDefault, image_url1, image_url2, image_url3, getIp, information, mounted, lucky}
        
    }
}

</script>


<style lang="css" scoped>
    img{
        display: block;
        margin-left: auto;
        margin-right: auto;
        max-width: 30%;
        height: 100%;
        flex: 30% 1 1; 
        margin: 2px;
        object-fit: contain;
    }    
    div {
        width: 100%;
        display: flex;
        margin:10px;
    }
    #controller{
        width: 100%;
        display: flex;
        margin:5px;
    }
    button{
        display: block;
    }
    #information{
        width: 100%;
        flex-direction: column;
        margin:5px;
    }
    #picture_preview{
        /* this is the pictures container */
        width: 100vw;
        display: flex;
        flex-direction: row;
        margin:5px;
        height: 30vh;
    }
</style>