// "use client";

import TitleBar from "../components/title-bar";
import CardStartAndLink from "../components/card-start-and-link";
import CardNotice from "../components/card-notice";

export default function PagePosterStart() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardStartAndLink></CardStartAndLink>
        <CardNotice></CardNotice>
      </div>
    </div>
  );
}
