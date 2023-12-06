// "use client";

import TitleBar from "../components/title-bar";
import CardPosterStartAndLink from "../components/poster/card-poster-start-and-link";
import CardPosterNotice from "../components/poster/card-poster-notice";

export default function PagePosterStart() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardPosterStartAndLink></CardPosterStartAndLink>
        <CardPosterNotice></CardPosterNotice>
      </div>
    </div>
  );
}
