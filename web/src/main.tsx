import React from "react";
import ReactDOM from "react-dom/client";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
import { NextUIProvider } from "@nextui-org/react";
import "./index.css";
import ErrorPage from "./error-page";
import PageRoot from "./routes/page-root";
import PagePoster from "./routes/page-poster";
import PagePosterStart from "./routes/page-poster-start";

const router = createBrowserRouter([
  {
    path: "/",
    element: (
      <NextUIProvider>
        <PageRoot></PageRoot>
      </NextUIProvider>
    ),
    errorElement: <ErrorPage></ErrorPage>,
  },
  {
    path: "/poster",
    element: (
      <NextUIProvider>
        <PagePoster></PagePoster>
      </NextUIProvider>
    ),
  },
  {
    path: "/poster_start",
    element: (
      <NextUIProvider>
        <PagePosterStart></PagePosterStart>
      </NextUIProvider>
    ),
  },
]);

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <RouterProvider router={router}></RouterProvider>
  </React.StrictMode>
);
