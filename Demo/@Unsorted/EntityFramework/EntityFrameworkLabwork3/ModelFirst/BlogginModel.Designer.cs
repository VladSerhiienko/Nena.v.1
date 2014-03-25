﻿//------------------------------------------------------------------------------
// <auto-generated>
//    This code was generated from a template.
//
//    Manual changes to this file may cause unexpected behavior in your application.
//    Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using System;
using System.ComponentModel;
using System.Data.EntityClient;
using System.Data.Objects;
using System.Data.Objects.DataClasses;
using System.Linq;
using System.Runtime.Serialization;
using System.Xml.Serialization;

[assembly: EdmSchemaAttribute()]
#region EDM Relationship Metadata

[assembly: EdmRelationshipAttribute("BlogginModel", "BlogPost", "Blog", System.Data.Metadata.Edm.RelationshipMultiplicity.One, typeof(ModelFirst.Blog), "Post", System.Data.Metadata.Edm.RelationshipMultiplicity.Many, typeof(ModelFirst.Post), true)]

#endregion

namespace ModelFirst
{
    #region Contexts
    
    /// <summary>
    /// No Metadata Documentation available.
    /// </summary>
    public partial class BlogginModelContainer : ObjectContext
    {
        #region Constructors
    
        /// <summary>
        /// Initializes a new BlogginModelContainer object using the connection string found in the 'BlogginModelContainer' section of the application configuration file.
        /// </summary>
        public BlogginModelContainer() : base("name=BlogginModelContainer", "BlogginModelContainer")
        {
            this.ContextOptions.LazyLoadingEnabled = true;
            OnContextCreated();
        }
    
        /// <summary>
        /// Initialize a new BlogginModelContainer object.
        /// </summary>
        public BlogginModelContainer(string connectionString) : base(connectionString, "BlogginModelContainer")
        {
            this.ContextOptions.LazyLoadingEnabled = true;
            OnContextCreated();
        }
    
        /// <summary>
        /// Initialize a new BlogginModelContainer object.
        /// </summary>
        public BlogginModelContainer(EntityConnection connection) : base(connection, "BlogginModelContainer")
        {
            this.ContextOptions.LazyLoadingEnabled = true;
            OnContextCreated();
        }
    
        #endregion
    
        #region Partial Methods
    
        partial void OnContextCreated();
    
        #endregion
    
        #region ObjectSet Properties
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        public ObjectSet<Blog> Blogs
        {
            get
            {
                if ((_Blogs == null))
                {
                    _Blogs = base.CreateObjectSet<Blog>("Blogs");
                }
                return _Blogs;
            }
        }
        private ObjectSet<Blog> _Blogs;
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        public ObjectSet<Post> Posts
        {
            get
            {
                if ((_Posts == null))
                {
                    _Posts = base.CreateObjectSet<Post>("Posts");
                }
                return _Posts;
            }
        }
        private ObjectSet<Post> _Posts;

        #endregion

        #region AddTo Methods
    
        /// <summary>
        /// Deprecated Method for adding a new object to the Blogs EntitySet. Consider using the .Add method of the associated ObjectSet&lt;T&gt; property instead.
        /// </summary>
        public void AddToBlogs(Blog blog)
        {
            base.AddObject("Blogs", blog);
        }
    
        /// <summary>
        /// Deprecated Method for adding a new object to the Posts EntitySet. Consider using the .Add method of the associated ObjectSet&lt;T&gt; property instead.
        /// </summary>
        public void AddToPosts(Post post)
        {
            base.AddObject("Posts", post);
        }

        #endregion

    }

    #endregion

    #region Entities
    
    /// <summary>
    /// No Metadata Documentation available.
    /// </summary>
    [EdmEntityTypeAttribute(NamespaceName="BlogginModel", Name="Blog")]
    [Serializable()]
    [DataContractAttribute(IsReference=true)]
    public partial class Blog : EntityObject
    {
        #region Factory Method
    
        /// <summary>
        /// Create a new Blog object.
        /// </summary>
        /// <param name="id">Initial value of the Id property.</param>
        /// <param name="name">Initial value of the Name property.</param>
        /// <param name="url">Initial value of the Url property.</param>
        public static Blog CreateBlog(global::System.Int32 id, global::System.String name, global::System.String url)
        {
            Blog blog = new Blog();
            blog.Id = id;
            blog.Name = name;
            blog.Url = url;
            return blog;
        }

        #endregion

        #region Simple Properties
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=true, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.Int32 Id
        {
            get
            {
                return _Id;
            }
            set
            {
                if (_Id != value)
                {
                    OnIdChanging(value);
                    ReportPropertyChanging("Id");
                    _Id = StructuralObject.SetValidValue(value, "Id");
                    ReportPropertyChanged("Id");
                    OnIdChanged();
                }
            }
        }
        private global::System.Int32 _Id;
        partial void OnIdChanging(global::System.Int32 value);
        partial void OnIdChanged();
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=false, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.String Name
        {
            get
            {
                return _Name;
            }
            set
            {
                OnNameChanging(value);
                ReportPropertyChanging("Name");
                _Name = StructuralObject.SetValidValue(value, false, "Name");
                ReportPropertyChanged("Name");
                OnNameChanged();
            }
        }
        private global::System.String _Name;
        partial void OnNameChanging(global::System.String value);
        partial void OnNameChanged();
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=false, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.String Url
        {
            get
            {
                return _Url;
            }
            set
            {
                OnUrlChanging(value);
                ReportPropertyChanging("Url");
                _Url = StructuralObject.SetValidValue(value, false, "Url");
                ReportPropertyChanged("Url");
                OnUrlChanged();
            }
        }
        private global::System.String _Url;
        partial void OnUrlChanging(global::System.String value);
        partial void OnUrlChanged();

        #endregion

        #region Navigation Properties
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [XmlIgnoreAttribute()]
        [SoapIgnoreAttribute()]
        [DataMemberAttribute()]
        [EdmRelationshipNavigationPropertyAttribute("BlogginModel", "BlogPost", "Post")]
        public EntityCollection<Post> Posts
        {
            get
            {
                return ((IEntityWithRelationships)this).RelationshipManager.GetRelatedCollection<Post>("BlogginModel.BlogPost", "Post");
            }
            set
            {
                if ((value != null))
                {
                    ((IEntityWithRelationships)this).RelationshipManager.InitializeRelatedCollection<Post>("BlogginModel.BlogPost", "Post", value);
                }
            }
        }

        #endregion

    }
    
    /// <summary>
    /// No Metadata Documentation available.
    /// </summary>
    [EdmEntityTypeAttribute(NamespaceName="BlogginModel", Name="Post")]
    [Serializable()]
    [DataContractAttribute(IsReference=true)]
    public partial class Post : EntityObject
    {
        #region Factory Method
    
        /// <summary>
        /// Create a new Post object.
        /// </summary>
        /// <param name="id">Initial value of the Id property.</param>
        /// <param name="title">Initial value of the Title property.</param>
        /// <param name="content">Initial value of the Content property.</param>
        /// <param name="blogId">Initial value of the BlogId property.</param>
        public static Post CreatePost(global::System.Int32 id, global::System.String title, global::System.String content, global::System.Int32 blogId)
        {
            Post post = new Post();
            post.Id = id;
            post.Title = title;
            post.Content = content;
            post.BlogId = blogId;
            return post;
        }

        #endregion

        #region Simple Properties
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=true, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.Int32 Id
        {
            get
            {
                return _Id;
            }
            set
            {
                if (_Id != value)
                {
                    OnIdChanging(value);
                    ReportPropertyChanging("Id");
                    _Id = StructuralObject.SetValidValue(value, "Id");
                    ReportPropertyChanged("Id");
                    OnIdChanged();
                }
            }
        }
        private global::System.Int32 _Id;
        partial void OnIdChanging(global::System.Int32 value);
        partial void OnIdChanged();
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=false, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.String Title
        {
            get
            {
                return _Title;
            }
            set
            {
                OnTitleChanging(value);
                ReportPropertyChanging("Title");
                _Title = StructuralObject.SetValidValue(value, false, "Title");
                ReportPropertyChanged("Title");
                OnTitleChanged();
            }
        }
        private global::System.String _Title;
        partial void OnTitleChanging(global::System.String value);
        partial void OnTitleChanged();
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=false, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.String Content
        {
            get
            {
                return _Content;
            }
            set
            {
                OnContentChanging(value);
                ReportPropertyChanging("Content");
                _Content = StructuralObject.SetValidValue(value, false, "Content");
                ReportPropertyChanged("Content");
                OnContentChanged();
            }
        }
        private global::System.String _Content;
        partial void OnContentChanging(global::System.String value);
        partial void OnContentChanged();
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [EdmScalarPropertyAttribute(EntityKeyProperty=false, IsNullable=false)]
        [DataMemberAttribute()]
        public global::System.Int32 BlogId
        {
            get
            {
                return _BlogId;
            }
            set
            {
                OnBlogIdChanging(value);
                ReportPropertyChanging("BlogId");
                _BlogId = StructuralObject.SetValidValue(value, "BlogId");
                ReportPropertyChanged("BlogId");
                OnBlogIdChanged();
            }
        }
        private global::System.Int32 _BlogId;
        partial void OnBlogIdChanging(global::System.Int32 value);
        partial void OnBlogIdChanged();

        #endregion

        #region Navigation Properties
    
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [XmlIgnoreAttribute()]
        [SoapIgnoreAttribute()]
        [DataMemberAttribute()]
        [EdmRelationshipNavigationPropertyAttribute("BlogginModel", "BlogPost", "Blog")]
        public Blog Blog
        {
            get
            {
                return ((IEntityWithRelationships)this).RelationshipManager.GetRelatedReference<Blog>("BlogginModel.BlogPost", "Blog").Value;
            }
            set
            {
                ((IEntityWithRelationships)this).RelationshipManager.GetRelatedReference<Blog>("BlogginModel.BlogPost", "Blog").Value = value;
            }
        }
        /// <summary>
        /// No Metadata Documentation available.
        /// </summary>
        [BrowsableAttribute(false)]
        [DataMemberAttribute()]
        public EntityReference<Blog> BlogReference
        {
            get
            {
                return ((IEntityWithRelationships)this).RelationshipManager.GetRelatedReference<Blog>("BlogginModel.BlogPost", "Blog");
            }
            set
            {
                if ((value != null))
                {
                    ((IEntityWithRelationships)this).RelationshipManager.InitializeRelatedReference<Blog>("BlogginModel.BlogPost", "Blog", value);
                }
            }
        }

        #endregion

    }

    #endregion

}
