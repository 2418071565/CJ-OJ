<template>
    <div class="daytheme">
        <el-menu
            :default-active="1"
            class="el-menu-demo"
            mode="horizontal"
            text-color="rgb(0,0,0)"
            active-text-color="#ffd04b"
            @select="handleSelect"
            router
            id="head-menu"
        >
            <el-menu-item index="/login">Processing Center</el-menu-item>
            <el-sub-menu index="2">
                <template #title>Workspace</template>
                <el-menu-item index="2-1">item one</el-menu-item>
                <el-menu-item index="2-2">item two</el-menu-item>
                <el-menu-item index="2-3">item three</el-menu-item>
                <el-sub-menu index="2-4">
                    <template #title>item four</template>
                    <el-menu-item index="2-4-1">item one</el-menu-item>
                    <el-menu-item index="2-4-2">item two</el-menu-item>
                    <el-menu-item index="2-4-3">item three</el-menu-item>
                </el-sub-menu>
            </el-sub-menu>
            <el-menu-item index="3" disabled>Info</el-menu-item>
            <el-menu-item index="4">Orders</el-menu-item>
        </el-menu>
        <div class="form">
            <div class="form-main">
                <div class="input" id="username">
                    <svg
                        viewBox="0 0 16 16"
                        fill="#2e2e2e"
                        height="30"
                        width="30"
                        xmlns="http://www.w3.org/2000/svg"
                        class="inputIcon"
                    >
                        <path
                            d="M13.106 7.222c0-2.967-2.249-5.032-5.482-5.032-3.35 0-5.646 
                            2.318-5.646 5.702 0 3.493 2.235 5.708 5.762 5.708.862 0 1.689-.123 
                            2.304-.335v-.862c-.43.199-1.354.328-2.29.328-2.926 0-4.813-1.88-4.813-4.798 
                            0-2.844 1.921-4.881 4.594-4.881 2.735 0 4.608 1.688 4.608 4.156 0 1.682-.554 2.769-1.416 
                            2.769-.492 0-.772-.28-.772-.76V5.206H8.923v.834h-.11c-.266-.595-.881-.964-1.6-.964-1.4
                            0-2.378 1.162-2.378 2.823 0 1.737.957 2.906 2.379 2.906.8 0 1.415-.39 
                            1.709-1.087h.11c.081.67.703 1.148 1.503 1.148 1.572 0 2.57-1.415 
                            2.57-3.643zm-7.177.704c0-1.197.54-1.907 1.456-1.907.93 0 1.524.738 1.524 1.907S8.308 
                            9.84 7.371 9.84c-.895 0-1.442-.725-1.442-1.914z"
                        ></path>
                    </svg>
                    <input
                        type="text"
                        class="inputField"
                        placeholder="Username"
                        v-model="loginForm.userName"
                    />
                </div>
                <div class="input" id="password">
                    <svg
                        viewBox="0 0 16 16"
                        height="25"
                        width="30"
                        xmlns="http://www.w3.org/2000/svg"
                        class="inputIcon"
                    >
                        <path
                            d="M8 1a2 2 0 0 1 2 2v4H6V3a2 2 0 0 1 2-2zm3 6V3a3 3 0 0 0-6 0v4a2 2 
                            0 0 0-2 2v5a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2z"
                        ></path>
                    </svg>
                    <input
                        type="password"
                        class="inputField"
                        placeholder="Password"
                        autocomplete="new-password"
                        
                    />
                </div>
                <div class="submit">
                    <button @click="" id="button-login">submit</button>
                </div>
            </div>
        </div>
    </div>
</template>
<script setup>
import { ref, onMounted } from "vue";
const loginForm = ref({
    userName: "",
    userStatus: 0,
    password: "",
    checkCode: "",
});

const registerForm = ref({
    userName: "",
    userStatus: 0,
    password: "",
    checkCode: "",
});
</script>
<style scoped>
* {
    padding: 0;
    margin: 0;
}
#head-menu {
    height: 60px;
}
.form {
    position: relative;
    height: 650px;
    width: 100%;
    border: 1px solid black;
}
.form-main {
    position: relative;
    margin: 75px 580px;
    height: 500px;
    width: 400px;
    border: 1px solid black;
    border-radius: 15px;
    background-color: white;
    display: flex;
    align-items: center;
    flex-direction: column;
    justify-content: center;
    gap: 10px;
}
.input {
    position: relative;
    height: 40px;
    width: 240px;
    border-radius: 30px;
    transition: all 0.3s;
}
.inputIcon {
    position: absolute;
    margin: 5px 1px;
    z-index: 1;
}
.inputField {
    width: 100%;
    height: 40px;
    background-color: transparent;
    border: none;
    border-bottom: 3px solid rgb(173, 173, 173);
    border-radius: 30px;
    color: black;
    font-size: 1em;
    font-weight: 500;
    box-sizing: border-box;
    padding-left: 30px;
}
.inputField:focus {
    outline: none;
    background-color: transparent;
    border-bottom: 3px solid rgb(199, 114, 255);
}
.submit{
    height: 40px;
    width: 240px;
}
#button-login{
    width: 100%;
    height: 100%;
    border-radius: 30px;
    outline: none;
    font-size: 2em;
    font-family: Arial, Helvetica, sans-serif;
}
@keyframes formChange {
    0% {
        transform: rotate(0deg);
    }
    100% {
        transform: rotate(90deg);
    }
}
/* 白天主题 */
.daytheme {
    position: absolute;
    left: 0;
    top: 0;
    height: 1000px;
    width: 100%;
    background-color: rgba(241, 240, 240, 0.5);
}
.daytheme #head-menu {
    background-color: white;
}
/* 夜晚主题 */
.nighttheme {
}
</style>
