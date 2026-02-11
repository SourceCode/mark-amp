import React from 'react';
import ReactMarkdown from 'react-markdown';
import remarkGfm from 'remark-gfm';
import MermaidBlock from './MermaidBlock';

interface PreviewProps {
  content: string;
}

const Preview: React.FC<PreviewProps> = ({ content }) => {
  return (
    <div className="prose prose-invert max-w-none p-6 font-sans text-win-text">
      <ReactMarkdown
        remarkPlugins={[remarkGfm]}
        components={{
          code({ node, inline, className, children, ...props }: any) {
            const match = /language-(\w+)/.exec(className || '');
            const isMermaid = match && match[1] === 'mermaid';

            if (!inline && isMermaid) {
              return <MermaidBlock chart={String(children).replace(/\n$/, '')} />;
            }

            return !inline && match ? (
              <div className="relative group">
                <div className="absolute right-2 top-2 text-xs text-win-muted opacity-50 font-mono">
                  {match[1]}
                </div>
                <pre className="bg-black/30 p-4 rounded border border-win-borderLight overflow-x-auto">
                  <code className={className} {...props}>
                    {children}
                  </code>
                </pre>
              </div>
            ) : (
              <code className="bg-win-accent/20 text-win-accent px-1.5 py-0.5 rounded text-sm font-mono" {...props}>
                {children}
              </code>
            );
          },
          // Customizing other elements for the theme
          h1: ({node, ...props}) => <h1 className="text-3xl font-bold text-win-accent border-b border-win-borderLight pb-2 mb-6" {...props} />,
          h2: ({node, ...props}) => <h2 className="text-2xl font-semibold text-win-accent mt-8 mb-4" {...props} />,
          h3: ({node, ...props}) => <h3 className="text-xl font-medium text-win-text mt-6 mb-3" {...props} />,
          p: ({node, ...props}) => <p className="mb-4 leading-relaxed text-win-text/90" {...props} />,
          a: ({node, ...props}) => <a className="text-win-accent hover:underline decoration-win-accent/50 underline-offset-4" {...props} />,
          blockquote: ({node, ...props}) => <blockquote className="border-l-4 border-win-accent pl-4 py-1 italic bg-win-accent/5 my-4" {...props} />,
          ul: ({node, ...props}) => <ul className="list-disc pl-6 mb-4 marker:text-win-accent" {...props} />,
          ol: ({node, ...props}) => <ol className="list-decimal pl-6 mb-4 marker:text-win-accent" {...props} />,
          table: ({node, ...props}) => <div className="overflow-x-auto my-6 border border-win-borderLight rounded"><table className="w-full text-left border-collapse" {...props} /></div>,
          th: ({node, ...props}) => <th className="bg-win-panel p-3 border-b border-win-borderLight font-semibold text-win-accent" {...props} />,
          td: ({node, ...props}) => <td className="p-3 border-b border-win-borderLight/30" {...props} />,
          hr: ({node, ...props}) => <hr className="border-win-borderLight my-8" {...props} />,
        }}
      >
        {content}
      </ReactMarkdown>
    </div>
  );
};

export default Preview;
