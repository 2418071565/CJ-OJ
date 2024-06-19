import { createRouter,createWebHistory } from "vue-router";
import login from "@/view/login.vue";

const routes = [
    {path:'/login',component:login},
]

const router = createRouter({
    history:createWebHistory(),
    routes:routes,
})


export default router;