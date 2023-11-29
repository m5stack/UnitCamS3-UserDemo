// "use client";

import TitleBar from "../components/title-bar";
import CardPosterConfig from "../components/card-poster-config";

export default function PagePoster() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardPosterConfig></CardPosterConfig>
      </div>
    </div>
  );
}
